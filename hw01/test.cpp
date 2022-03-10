#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

// mask of ones with length n
constexpr uint64_t ones( const uint8_t n )
{
  return ( 1ull << n ) - 1;
}

// struct for storing bit array of variable size ( max 32bit )
// ( 0, 0 ) reserved to signal invalid value
// ( 1, 0 ) reserved to signal EOF
struct bitArray
{
  uint32_t bits;
  uint8_t length;

  bitArray()
    : bits( 0 ), length( 0 )
  {};
  
  bitArray( uint32_t value, uint8_t length )
    : bits( value & ( ones( length ) | 1 ) ), length( length )
  {};
  
  bool operator==( const bitArray &other ) const
  {
    return !( *this > other ) && !( *this < other );
  };

  bool operator!=( const bitArray &other ) const
  {
    return !( *this == other );
  };

  uint32_t operator&( uint32_t other ) const
  {
    return bits & other;
  };

  bitArray getFirst( uint8_t count ) const
  {
    bitArray res( *this );
    res.bits >>= res.length - count;
    res.length = count;
    return res;
  };

  bitArray &operator+=( const bitArray &other )
  {
    return *this = *this + other;
  };

  bitArray &operator+=( const uint8_t other )
  {
    return *this = *this + bitArray( other, 8 );
  };

  bitArray operator+( const bitArray &other ) const
  {
    bitArray res( *this );
    res.bits <<= other.length;
    res.bits |= other.bits & ones(other.length);
    res.length += other.length;
    return res;
  };

  // needed for usage as key in map
  bool operator<( const bitArray &other ) const
  {
    return bits == other.bits ? length < other.length : bits < other.bits;
  };

  bool operator>( const bitArray &other ) const
  {
    return other < *this;
  };

  operator bool() const
  {
    return length;
  };

  /**
   * @brief Prints contents to stream
   */
  void print( ofstream &stream ) const
  {
    for( int8_t shift = length - 8; shift >= 0; shift -= 8 )
    {
      const uint8_t temp = bits >> shift;
      stream.put( temp );
    }
  };
};

/**
 * @brief class for I/O operations with individual bits
 */
class bitStream
{
public:
  const static uint8_t BUFFER_SIZE = 64;
  bitStream( fstream &stream )
    : buffer( 0 ), bufferLen( 0 ), stream( stream )
  {};

  /**
   * @brief Pops length of bits from stream
   * 
   * @param length number of bits
   */
  bitArray get( const uint8_t length )
  {
    fillBuffer();
    if( length > bufferLen ) return bitArray(); // invalid bitStream: eof or file during read
    uint64_t temp = buffer;
    temp >>= bufferLen - length;
    bufferLen -= length;
    buffer &= ones( bufferLen );    // discard front bits that have been read
    return bitArray( temp, length );
  };

  /**
   * @brief Write bits to stream
   */
  void write( const bitArray &data )
  {
    buffer <<= data.length;
    buffer |= data.bits;
    bufferLen += data.length;
    while( bufferLen >= 8 ) writeByte();
  };

  /**
   * @brief Padds bits in buffer with zeros to multiple of byte
   */
  void flush()
  {
    if( !bufferLen ) return;
    buffer <<= 8 - bufferLen;
    bufferLen = 8;
    writeByte();
  };

  /**
   * @brief Extracts UTF-8 byte representation of character to target
   * 
   * @return true on success
   * @return false bad UTF-8 format in stream
   */
  bool getUtf8( bitArray &target )
  {
    bitArray first = get( 8 );
    if( !first ) return false;

    // 0b<3 zero bytes> 1110 XXXX ---> ( << 24 ) 
    // 0b1110 XXXX <3 zero bytes> ---> ~
    // 0b0001 XXXX <3 ones bytes> ---> clz: number of leading zeros
    uint8_t byteCount = __builtin_clz( ~( first.bits << 24 ) );

    // 1 means first == 10XX XXXX (intermediate byte as first byte)
    if( byteCount == 1 || byteCount > 4 ) return false;

    for( uint8_t byte = 1; byte < byteCount; ++byte )
    {
      bitArray intermediate = get( 8 );
      if( intermediate.getFirst( 2 ) != bitArray( 0b10, 2 ) )
        return false;
      first += intermediate;
    }

    /* 0xf4908080 ==
     * 0b 1111 0100
     *    1001 0000
     *    1000 0000
     *    1000 0000 ==
     * max UTF-8 value
     */
    if( first.bits >= 0xf4908080ul ) return bitArray();
    target = first;
    return true;
  };
  
private:
  /**
   * @brief Fills internal buffer from underlying stream as much as possible
   */
  void fillBuffer()
  {
    while( bufferLen + 8 <= BUFFER_SIZE )
    {
      uint8_t newByte = stream.get();
      if( !stream.good() ) return;
      buffer <<= 8;
      buffer |= newByte;
      bufferLen += 8;
    }
  };

  /**
   * @brief Writes byte to underlying stream
   */
  void writeByte()
  {
    uint64_t temp = buffer;
    bufferLen -= 8;
    buffer &= ones( bufferLen );
    temp >>= bufferLen;
    stream.put( temp );
  };

  // bufferLen = 7 means: buffer = b'... XXXX XXXX X100 1010'
  uint64_t buffer;
  uint8_t bufferLen; //size in bits
  fstream &stream;
};


class huffmanCode
{
public:
  huffmanCode()
    :top( nullptr )
  {};

  ~huffmanCode()
  {
    if( top ) top->deleteNode();
  };

  bool decompress( const char *inFileName, const char *outFileName )
  {
    fstream inFile( inFileName, ifstream::in | ifstream::binary );
    if( !inFile.good() ) return false;

    ofstream output( outFileName, fstream::binary | fstream::trunc );
    if( !output.good() ) return false; 

    bitStream input( inFile );

    if( !loadTree( input ) ) return false;

    if( !readChunks( input, output ) ) return false;

    // inFile can fail because of EOF but cant have io problem
    if( inFile.bad() || output.fail() ) return false;

    return true;
  };

  bool compress( const char *inFileName, const char *outFileName )
  {
    ifstream inFile( inFileName, ifstream::binary );
    if( !inFile.good() ) return false;

    if( !createTreeFrom( inFile ) ) return false;

    fstream outFile( outFileName, fstream::out | fstream::binary | fstream::trunc );
    if( !outFile.good() ) return false; 

    bitStream output( outFile );

    printHuffCode( output );
    while( printChunk( inFile, output ) );

    output.flush();

    // inFile can fail because of EOF but cant have io problem
    if( inFile.bad() || outFile.fail() ) return false;
    return true;
  };

private:
  /**
   * @brief structure for nodes of huffman tree
   * 0: left
   * 1: right
   * left == right == nullptr means that node is leaf and utf8 stores
   * valid utf8 bytes for series of bits from top to current node
   */
  struct node
  {
    node *parent;
    node *left;
    node *right;
    bitArray utf8;

    node()
      : parent( nullptr), left( nullptr ), right( nullptr )
    {};

    // leaf node constructor
    node( const bitArray &utf8 )
      : parent( nullptr), left( nullptr ), right( nullptr ), utf8( utf8 )
    {};

    // parent node constructor
    node( node *left, node *right )
      : parent( nullptr ), left( left ), right ( right )
    {};

    /**
     * @brief Creates subtree from this node
     * 
     * @return true on success
     * @return false on corrupted UTF-8 characted / EOF / io-error 
     */
    bool appendNode( bitStream &input )
    {
      bitArray first = input.get( 1 );
      if( first == bitArray( 0, 1 ) )
      {
        left = new node;
        right = new node;
        return left->appendNode( input ) && right->appendNode( input );
      }
      else if( first == bitArray( 1, 1 ) )
        return input.getUtf8( utf8 );
      else return false;
    };

    /**
     * @brief free's node memory
     */
    void deleteNode()
    {
      if( left ) left->deleteNode();
      if( right ) right->deleteNode();
      delete this;
    };

    /**
     * @brief Prints huffman tree to compressed file
     */
    void printTree( bitStream &output ) const
    {
      if( left && right )
      {
        output.write( bitArray( 0, 1 ) );
        left->printTree( output );
        right->printTree( output );
        return;
      }
      output.write( bitArray( 1, 1 ) );
      output.write( utf8 );
    };

    /**
     * @brief Prints huffman code of current node
     * Works by recursively calls parent to top and writes to output while returning
     */
    void printCode( bitStream &output ) const
    {
      if( parent )
      {
        parent->printCode( output );
        if( parent->left == this ) output.write( bitArray( 0, 1 ) );
        if( parent->right == this ) output.write( bitArray( 1, 1) );
      }
    };
  };

/* VVV DECOMPRESSION PRIVATE METHODS VVV */

  /**
   * @brief Loads huffman tree from file
   * 
   * @return true on success
   * @return false on corrupted UTF-8 characted / EOF / io-error 
   */
  bool loadTree( bitStream &input )
  {
    top = new node;
    return top->appendNode( input );
  };

  /**
   * @brief Read chunks from input and decodes them
   * Chunk starting 0b1 means 4096 compressed UTF-8 chars follows
   * Chunk starting 0b0 is followed by 12bits representing count of UTF-8 chars
   * @return true on success
   * @return false bad/corrupted input format
   */
  bool readChunks( bitStream &input, ofstream &output ) const
  {
    bitArray chunkType = input.get( 1 );
    while( chunkType == bitArray( 1, 1 ) )
    {
      if( !readChunk( input, output ) ) return false;
      chunkType = input.get( 1 );
    }

    if( chunkType != bitArray( 0, 1 ) )
      return false;

    bitArray chunkSize = input.get( 12 );
    if( !chunkSize ) return false;
    if( !readChunk( input, output, chunkSize.bits ) ) return false;

    return input.get( 8 ) == bitArray(); // input file cant contain one whole useless byte
  };

  /**
   * @brief Reads huffman code from input and prints decoded data to output
   * 
   * @param chunkSize count of UTF-8 chracters needed at output
   * @return true on succes
   * @return false EOF, io-error occured before chuckSize characters could be read
   */
  bool readChunk( bitStream &input, ofstream &output, uint16_t chunkSize = 4096 ) const
  {
    while( chunkSize )
    {
      node *current = top;
      while( current->right || current->left )
      {
        bitArray dir = input.get( 1 );
        if( dir == bitArray( 0, 1 ) )
          current = current->left;
        else if( dir == bitArray( 1, 1 ) )
          current = current->right;
        else return false;
      }
      current->utf8.print( output );
      --chunkSize;
    }
    return true;
  };

/* VVV COMPRESSION PRIVATE METHODS VVV */

  /**
   * @brief Create a huffman tree/code to compress input
   * 
   * @return true on success
   * @return false on bad UTF-8 coding or IO-error
   */
  bool createTreeFrom( ifstream &input )
  {
    map<bitArray, uint64_t> characterCounts;
    if( !charFrequency( input, characterCounts ) )
      return false;

    // map can't be sorted
    vector< pair<node *, uint64_t > > charArray;
    charArray.reserve( characterCounts.size() );

    for( const pair<bitArray, uint64_t> &letter: characterCounts )
    {
      node *leaf = new node( letter.first );
      charArray.push_back( pair<node *, uint64_t>( leaf, letter.second ) );
      lookUpTable[ letter.first ] = leaf;
    }

    sort( charArray.begin(), charArray.end(),
          []( const pair<node *, uint64_t> &a, const pair<node *, uint64_t> &b )
          {
             return a.second < b.second;
          } );


    queue< pair<node *, uint64_t> > source, target;
    for( const pair<node *, uint64_t> &letter: charArray )
      source.push( letter );

    /* from ordered queue ( a_1, a_2, a_3, a_4 ... a_n ) we want to merge to a_123...
    * we need find two smallest elems( a_i, a_j ) fast so queue needs to be always ordered
    * removal of those elems and insertion of newly merged a_ij also needs to be fast
    * we can achieve this by having two ordered queues source -> target
    * two smallest elements will be either (a_1, a_2)/(a_1, b_1)/(b_1, b_2)
    * we always pick smallest elems so each new merged elem is greater or equal than previous
    * insertion will always happen to end of target to maintain order
    * after some steps source will become empty, then
    * after n - 1 steps there will be only single element in target
    */

    // iterate until we have only one elem in target ( no in source )
    while( target.size() > 1 || source.size() )
      target.push( merge( popSmallest( source, target ),
                          popSmallest( source, target ) ) );

    top = target.front().first;
    return true;
  };

  /**
   * @brief Stores UTF-8 character frequencies to charFreq
   * 
   * @param charFreq map for storing <UTF-8, count> pairs
   * @return true on success
   * @return false on bad UTF-8 coding or IO-error
   */
  static bool charFrequency( ifstream &input, map<bitArray, uint64_t> &charFreq )
  {
    while( true )
    {
      bitArray symbol = getUtf8( input );
      if( !symbol )
      {
        input.clear();
        input.seekg( 0 );
        return symbol == bitArray( 1, 0 );
      }
      charFreq[ symbol ]++;
    }
  };

  /**
   * @brief Get the Utf8 bytes from input
   * Warning: function gets UTF-8 representation not unicode since that is almost not needed
   */
  static bitArray getUtf8( ifstream &input )
  {
    uint32_t first = input.get();
    if( !input.good() )
      return bitArray( !input.bad(), 0 );

    uint8_t byteCount = __builtin_clz( ~( first << 24 ) );
    if( byteCount == 1 || byteCount > 4 )
      return bitArray();

    bitArray utf8( first, 8 );

    for( uint8_t byte = 1; byte < byteCount; ++byte )
    {
      uint8_t trail = input.get();
      if( !input.good() ) return bitArray();
      if( ( trail & ( 0b11 << 6 ) ) != ( 1 << 7 ) ) return bitArray();
      utf8 += trail;
    }

    if( utf8.bits >= 0xf4908080ul ) return bitArray();
    return utf8;
  };

  /**
   * @brief Pop and return smallest element from two sorted queues
   * comparision is made on second item of pair ( occurence )
   * @return pair<node *, uint64_t> pair with smallest .second
   */
  static pair<node *, uint64_t> popSmallest( queue< pair<node *, uint64_t> > &queue_a,
                                             queue< pair<node *, uint64_t> > &queue_b )
  {
    pair<node *, uint64_t> temp;
    if( !queue_a.size() ) // no elems in queue_a
    {
      temp = queue_b.front();
      queue_b.pop();
      return temp;
    }
    if( !queue_b.size() ) // no elems in queue_b
    {
      temp = queue_a.front();
      queue_a.pop();
      return temp;
    }
    // there is at least one elem in each queue
    if( compPair( queue_a.front(), queue_b.front() ) <= 0 )
    {
      temp = queue_a.front();
      queue_a.pop();
      return temp;
    }
    temp = queue_b.front();
    queue_b.pop();
    return temp;
  };
  
  /**
   * @brief compair occurence of < node, int > pair
   * 
   * @return 1 if a > b, -1 if a < b, 0 otherwise
   */
  inline static int8_t compPair( const pair<node *, uint64_t> &a,
                                 const pair<node *, uint64_t> &b )
  {
    return ( a.second > b.second ) - ( b.second > a.second );
  };

  /**
   * @brief Create pair < parent node, combined occurence >
   * 
   * @param a < child node, occurence >
   * @param b < child node, occurence >
   * @return pair<node *, uint64_t> 
   */
  static pair<node *, uint64_t> merge( pair<node *, uint64_t> a,
                                       pair<node *, uint64_t> b )
  {
    pair<node *, uint64_t> temp( new node( a.first, b.first ), a.second + b.second );
    a.first->parent = temp.first;
    b.first->parent = temp.first;
    return temp;
  };

  /**
   * @brief prints Huffman tree mapping UTF-8 <-> huffman code
   */
  void printHuffCode( bitStream &output ) const
  {
    if( !top ) return;
    top->printTree( output );
  };

  /**
   * @brief compresses up to 4096 UTF-8 characters
   * 
   * @return true if there is still more data in inFile
   * @return false on EOF
   */
  bool printChunk( ifstream &inFile, bitStream &output ) const
  {
    // chunk = {0/1} <block>
    // block = bit representation for <0, 4096> utf-8 characters
    vector<bitArray> block;
    block.reserve( 4096 );

    for( uint16_t i = 0; i < 4096; ++i )
    {
      const bitArray symbol = getUtf8( inFile );
      if( !symbol ) break; // eof
      block.push_back( symbol );
    }

    if( block.size() < 4096 )
    {
      output.write( bitArray( 0, 1 ) );
      output.write( bitArray( block.size(), 12 ) );
    }
    else output.write( bitArray( 1, 1 ) );

    printBlock( block, output );

    return block.size() == 4096;
  };

  /**
   * @brief prints compressed data to output
   */
  void printBlock( const vector<bitArray> &data, bitStream &output ) const
  {
    for( const bitArray &symbol: data )
    {
      lookUpTable.at( symbol )->printCode( output );
    }
  };

  /**
   * @brief huffman code  --->  UTF-8
   * top of huffman tree
   */
  node *top;

  /**
   * @brief UTF-8  --->  huffman code
   * table stores ptr only to leaf that stores utf8 bytes as opposed to
   * pointing directly to bitArray which could be directly printed
   * this is because bitArray is limited to length of 32 bits whereas
   * huffman tree can be highly unbalanced
   * for example occurances { 1, 2, 4, 8, ... 2^n } of characters
   * produces tree with depth n-1, which means that it would be needed
   * n-2 bits to represent all codes.
   * For n = 35 codes wouldn't fit to bitArray, this could happen for file with size ~68.7GB
   */
  map< bitArray, node * > lookUpTable;

};


bool decompressFile ( const char * inFileName, const char * outFileName )
{
  huffmanCode huff;
  return huff.decompress( inFileName, outFileName );
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  huffmanCode huff;
  return huff.compress( inFileName, outFileName );
}

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  ifstream f1 ( fileName1, ifstream::binary | ifstream::ate );
  ifstream f2 ( fileName2, ifstream::binary | ifstream::ate );

  if( f1.fail() || f2.fail() )
    return false; //file problem

  if( f1.tellg() != f2.tellg() )
    return false; //size mismatch

  //seek back to beginning and use std::equal to compare contents
  f1.seekg( 0, ifstream::beg );
  f2.seekg( 0, ifstream::beg );
  return equal( istreambuf_iterator<char>( f1.rdbuf() ),
                istreambuf_iterator<char>(),
                istreambuf_iterator<char>( f2.rdbuf() ));
}

int main ( void )
{
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test0.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test1.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test2.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test3.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test4.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra0.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra1.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra2.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra3.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra4.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra5.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra6.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra7.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra8.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra9.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  assert ( !decompressFile ( "tests/in_4531057.bin", "tempfile" ) );

  assert ( compressFile ( "tests/random1-utf8.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/random1-utf8.orig", "tempfile" ) );

  assert ( compressFile ( "tests/random2-utf8.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/random2-utf8.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
