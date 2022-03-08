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
    return bits == other.bits && length == other.length;
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
    bitArray res = *this;
    res.bits >>= res.length - count;
    res.length = count;
    return res;
  };

  void operator+=( const bitArray &other )
  {
    bits <<= other.length;
    bits |= other.bits & ones(other.length);
    length += other.length;
  };

  void operator+=( const uint8_t other )
  {
    bits <<= 8;
    bits |= other;
    length += 8;
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
    return bits < other.bits || ( bits == other.bits && length < other.length );
  };

  operator bool() const
  {
    return length;
  };

  void print( ofstream &stream ) const
  {
    for( int8_t shift = length - 8; shift >= 0; shift -= 8 )
    {
      const uint8_t temp = bits >> shift;
      stream.write( (const char *)&temp, 1 );
    }
  };
};


class bitStream
{
public:
  const static uint8_t BUFFER_SIZE = 64;
  bitStream( fstream &stream)
    : prefix( 0u ), bufferLen( 0u ), stream( stream )
  {};

  bitArray get( const uint8_t length )
  {
    fillBuffer();
    // invalid bitStream: eof or file during read
    if( length > bufferLen ) return bitArray(); 
    uint64_t temp = prefix;
    temp >>= bufferLen - length;
    bufferLen -= length;
    prefix &= ones( bufferLen );    // discard front bits that have been read
    return bitArray( temp, length );
  };

  void write( const bitArray &data )
  {
    prefix <<= data.length;
    prefix |= data.bits;
    bufferLen += data.length;
    while( bufferLen >= 8 ) writeByte();
  };

  void flush()
  {
    if( !bufferLen ) return;
    prefix <<= 8 - bufferLen;
    bufferLen = 8;
    writeByte();
  };

  bool getUnicode( bitArray &target )
  {
    bitArray first = get( 8 );
    if( !first ) return false;

    // 0b<3 zero bytes> 1110 XXXX ---> ( << 24 ) 
    // 0b1110 XXXX <3 zero bytes> ---> ~
    // 0b0001 XXXX <3 ones bytes> ---> ~

    uint8_t byteCount = __builtin_clz( ~( first.bits << 24 ) );
    if( byteCount == 1 || byteCount > 4 ) return false;

    for( uint8_t byte = 1; byte < byteCount; ++byte )
    {
      bitArray trail = get( 8 );
      if( trail.getFirst( 2 ) == bitArray( 0b10, 2 ) )
        first += trail;
      else return false;
    }
    if( first.bits >= 0xf4908080ul ) return bitArray();
    target = first;
    return true;
  };
  
private:
  void fillBuffer()
  {
    while( bufferLen + 8 <= BUFFER_SIZE )
    {
      uint8_t newByte;
      stream.read( (char *)&newByte, 1 );
      if( !stream.good() ) return;
      prefix <<= 8;
      prefix |= newByte;
      bufferLen += 8;
    }
  };

  void writeByte()
  {
    uint64_t temp = prefix;
    bufferLen -= 8;
    prefix &= ones( bufferLen );
    temp >>= bufferLen;
    stream.write( (char *)&temp, 1 );
  };

  // bufferLen = 7 means: prefix = b'... XXXX XXXX X100 1010'
  uint64_t prefix;
  uint8_t bufferLen; //size in bits
  fstream &stream;
};


class huffmanTree
{
public:
  huffmanTree()
    :top( nullptr )
  {};

  ~huffmanTree()
  {
    if( top ) top->deleteNode();
  };

  bool loadTree( bitStream &input )
  {
    top = new node;
    return top->appendNode( input );
  };

  bool createTreeFrom( ifstream &input )
  {
    map<bitArray, uint64_t> characterCounts;
    if( !charFrequency( input, characterCounts ) )
      return false;

    if( input.bad() ) return false;
    input.clear();
    input.seekg( 0 );

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
      source.push( move(letter) );

/* from ordered queue ( a_1, a_2, a_3, a_4 ... a_n ) we want to merge to a_123...
 * we need find two smallest elems( a_i, a_j ) fast so queue needs to be always ordered
 * removal of those elems and insertion of newly merged a_ij also needs to be fast
 * we can achieve this by having two ordered queues source -> target
 * two smallest elements will be either (a_1, a_2)/(a_1, b_1)/(b_1, b_2)
 * we always pick smallest elems so each new merged is greater or equal than previous
 * insertion will always happen to end of target to maintain order
 * after some steps source will become empty
 * after n - 1 steps there will be only single element in target
 */

    // iterate until we have only one elem in target ( no in source )
    while( target.size() > 1 || source.size() )
      target.push( merge( getSmallest( source, target ),
                          getSmallest( source, target ) ) );

    top = target.front().first;
    return true;
  };

  bool decompress( bitStream &input, ofstream &output ) const
  {
    bitArray chunkType = input.get( 1 );
    uint16_t chunkSize;
//    bool firstChunk = true;
    while( chunkType == bitArray( 1, 1 ) )
    {
      if( !readChunk( input, output ) ) return false;
      chunkType = input.get( 1 );
//      firstChunk = false;
    }
    if( chunkType != bitArray( 0, 1 ) )
      return false;

    bitArray chunkSizeInfo = input.get( 12 );
    if( !chunkSizeInfo ) return false;
    chunkSize = chunkSizeInfo.bits;
//    if( firstChunk && !chunkSize ) return false;
    if( !readChunk( input, output, chunkSize ) ) return false;
    if( input.get( 8 ) != bitArray() ) return false; // input file cant contain one whole useless byte
    return true;
  };

  bool compress( ifstream &inFile, bitStream &output ) const
  {
    printHuffCode( output );
    while( printChunk( inFile, output ) );

    output.flush();
    return true;
  };

private:
  struct node
  {
    node *parent;
    node *left;
    node *right;
    bitArray unicode;
//    uint64_t count;
//    uint32_t unicode;

    node()
      : parent( nullptr), left( nullptr ), right( nullptr )
    {};

    node( const bitArray &unicode )
      : parent( nullptr), left( nullptr ), right( nullptr ), unicode( unicode )
    {};

    node( node *left, node *right )
      : parent( nullptr ), left( left ), right ( right )
    {};

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
        return input.getUnicode( unicode );
      else return false;
    };

    void deleteNode()
    {
      if( left ) left->deleteNode();
      if( right ) right->deleteNode();
      delete this;
    };

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
      output.write( unicode );
    };

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
                       
  inline static int8_t compPair( const pair<node *, uint64_t> &a,
                                 const pair<node *, uint64_t> &b )
  {
    return ( a.second > b.second ) - ( b.second > a.second );
  };

  static pair<node *, uint64_t> merge( pair<node *, uint64_t> a,
                                       pair<node *, uint64_t> b )
  {
    pair<node *, uint64_t> temp( new node( a.first, b.first ), a.second + b.second );
    a.first->parent = temp.first;
    b.first->parent = temp.first;
    return temp;
  };

  static pair<node *, uint64_t> getSmallest( queue< pair<node *, uint64_t> > &queue_a,
                                             queue< pair<node *, uint64_t> > &queue_b )
  {
    pair<node *, uint64_t> temp;
    if( !queue_a.size() ) // no elems in queue_a
    {
      temp = move( queue_b.front() );
      queue_b.pop();
      return temp;
    }
    if( !queue_b.size() ) // no elems in queue_b
    {
      temp = move( queue_a.front() );
      queue_a.pop();
      return temp;
    }
    // there is at least one elem in each queue
    if( compPair( queue_a.front(), queue_b.front() ) <= 0 )
    {
      temp = move( queue_a.front() );
      queue_a.pop();
      return temp;
    }
    temp = move( queue_b.front() );
    queue_b.pop();
    return temp;
  };

  static bool charFrequency( ifstream &input, map<bitArray, uint64_t> &charFreq )
  {
    while( true )
    {
      bitArray unicodeChar = getUnicode( input );
      if( !unicodeChar )
      {
        return unicodeChar.bits == 0b1;
      }
      charFreq[ unicodeChar ]++;
    }
  };

  static bitArray getUnicode( ifstream &input )
  {
    uint32_t first = 0;
    input.read( (char *)&first, 1 );
    if( !input.good() )
      return bitArray( input.eof(), 0 );

    uint8_t byteCount = __builtin_clz( ~( first << 24 ) );
    if( byteCount == 1 || byteCount > 4 )
      return bitArray();

    bitArray unicode( first, 8 );

    for( uint8_t byte = 1; byte < byteCount; ++byte )
    {
      uint8_t trail;
      input.read( (char *)&trail, 1 );
      if( !input.good() ) return bitArray();
      if( ( trail & ( 0b11 << 6 ) ) != ( 1 << 7 ) ) return bitArray();
      unicode += trail;
    }

    if( unicode.bits >= 0xf4908080ul ) return bitArray();
    return unicode;
  };

  void printHuffCode( bitStream &output ) const
  {
    if( !top ) return;
    top->printTree( output );
  };

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
      current->unicode.print( output );
      --chunkSize;
    }
    return true;
  };

  bool printChunk( ifstream &inFile, bitStream &output ) const
  {
    vector<bitArray> chunk;
    chunk.reserve( 4096 );

    for( uint16_t i = 0; i < 4096; ++i )
    {
      bitArray unicodeChar = getUnicode( inFile );
      if( !unicodeChar ) break; // eof
      chunk.push_back( move(unicodeChar) );
    }

    if( chunk.size() < 4096 )
    {
      output.write( bitArray( 0, 1 ) );
      output.write( bitArray( chunk.size(), 12 ) );
      printBlock( chunk, output );
      return false;
    }
    output.write( bitArray( 1, 1 ) );
    printBlock( chunk, output );
    return true;
  };

  void printBlock( const vector<bitArray> &data, bitStream &output ) const
  {
    for( const bitArray &unicodeChar: data )
    {
      lookUpTable.at( unicodeChar )->printCode( output );
    }
  };

  node *top;
  map< bitArray, node * > lookUpTable;
};


bool decompressFile ( const char * inFileName, const char * outFileName )
{
  fstream inFile( inFileName, ifstream::in | ifstream::binary );
  if( !inFile.good() ) return false;

  ofstream outFile( outFileName, fstream::binary | fstream::trunc );
  if( !outFile.good() ) return false; 

  bitStream input( inFile );

  huffmanTree huff;

  if( !huff.loadTree( input ) ) return false;
  if( !huff.decompress( input, outFile ) ) return false;

  // inFile can fail because of EOF but cant have io problem
  if( inFile.bad() || outFile.fail() ) return false;

  return true;
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  ifstream inFile( inFileName, ifstream::binary );
  if( !inFile.good() ) return false;

  huffmanTree huff;

  if( !huff.createTreeFrom( inFile ) ) return false;

  fstream outFile( outFileName, fstream::out | fstream::binary | fstream::trunc );
  if( !outFile.good() ) return false; 

  bitStream output( outFile );

  if( !huff.compress( inFile, output ) ) return false;

  // inFile can fail because of EOF but cant have io problem
  if( inFile.bad() || outFile.fail() ) return false;

  return true;
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
