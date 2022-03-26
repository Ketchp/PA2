/*
 *  Program for compressing and decompressing files using Huffman coding.
 */

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

/**
 * @param n length of mask
 *
 * @return Bitmask with n least significant set bits.
 */
constexpr uint64_t ones( const uint8_t n )
{
  return ( 1ull << n ) - 1;
}

/** \struct bitArray
 *  \brief Structure for storing bit array of variable size.
 *         Maximum array length is 64 bits.
 *         { bits, length }
 * 
 *  \remark ( 0, 0 ) reserved to signal invalid value.( Empty bitArray )
 *  \remark ( 1, 0 ) reserved to signal EOF.
 *  \remark Internall bit field representation is alligned to LSB.
 *  \remark Members are publicly available for easier manipulation.
 * 
 *  \warning If most significant set bit of bits is higer than length,
 *           all operations have undefined behaviour( this includes EOF state )
 *           This undefined state can be easily achieved by directly modifying members.
 */
struct bitArray
{
  uint64_t bits = 0;
  uint8_t length = 0;

  /**
   * @brief Invalid value constructor.
   */
  bitArray()
  {};

  /**
   * @brief Construct with value and length.
   * @remark Value set bits higher than length are cleared, except LSB to allow EOF construction.
   */
  bitArray( uint64_t value, uint8_t length )
    : bits( value & ( ones( length ) | 1 ) ), length( length )
  {};

  /**
   * @remark Validity of operation is guarateed by properties of <
   *
   * @return true if arguments are identical.
   * @return false if arguments differ.
   */
  friend bool operator==( const bitArray &lhs, const bitArray &rhs )
  {
    return lhs.bits == rhs.bits && lhs.length == rhs.length;
  };

  /**
   * @return true if arguments differ.
   * @return false if arguments are identical.
   */
  friend bool operator!=( const bitArray &lhs, const bitArray &rhs )
  {
    return !( lhs == rhs );
  };

  friend bitArray operator&( const bitArray &lhs, uint64_t rhs )
  {
    bitArray temp( lhs );
    temp.bits &= rhs;
    return temp;
  };

  /**
   * @remark Bits higher than highest bit of array are cleared.
   */
  friend bitArray operator|( const bitArray &lhs, uint64_t rhs )
  {
    bitArray temp( lhs );
    temp.bits |= rhs & ones( lhs.length );
    return temp;
  };

  bitArray &operator&=( uint64_t rhs )
  {
    return *this = *this & rhs;
  }

  /**
   * @remark Bits higher than highest bit of array are cleared.
   */
  bitArray &operator|=( uint64_t rhs )
  {
    return *this = *this & rhs;
  }

  /**
   * @param count of bits ( length ).
   * @return bitArray of first bits.
   */
  bitArray peek( uint8_t count = 1 ) const
  {
    bitArray res( *this );
    res.bits >>= res.length - count;
    res.length = count;
    return res;
  };

  /**
   * @brief Appends/Concatenates other bitArray.
   */
  bitArray &operator+=( const bitArray &other )
  {
    return *this = *this + other;
  };

  /**
   * @brief Appends/Concatenates 8 bits.
   */
  bitArray &operator+=( const uint8_t other )
  {
    return *this = *this + bitArray( other, 8 );
  };

  /**
   * @brief Concatenates bitArrays.
   */
  friend bitArray operator+( const bitArray &lhs, const bitArray &rhs )
  {
    bitArray res( lhs );
    res <<= rhs.length;
    return res |= rhs.bits;
  };

  bitArray &operator<<=( int shift )
  {
    return *this = *this << shift;
  }

  friend bitArray operator<<( const bitArray &lhs , int shift )
  {
    bitArray copy( lhs );
    copy.bits <<= shift;
    copy.length += shift;
    return copy;
  }

  /**
   * @brief Unspecified asymetric transitive relation.
   * 
   * @remark Oparation introduced for use in algorithms / data structures which needs
   * sorting / comparision operator.
   */
  friend bool operator<( const bitArray &lhs, const bitArray &rhs )
  {
    return lhs.bits == rhs.bits ? lhs.length < rhs.length : lhs.bits < rhs.bits;
  };

  /**
   * @brief Unspecified asymetric transitive relation.
   */
  friend bool operator>( const bitArray &lhs, const bitArray &rhs )
  {
    return rhs < lhs;
  };

  /**
   * @return true for valid bitArrays.
   * @return false for empty bitArray / EOF.
   */
  operator bool() const
  {
    return length;
  };

  // more operator could be easily implemented which would make sense ~, >=, <=, maybe []
  // ...but I only needed those up there and some of them are not even used

  /**
   * @return bitArray of extracted bits from front.
   */
  bitArray pop( uint8_t count = 1 )
  {
    bitArray result( *this );
    shrink( count );
    return result.peek( count );
  }

  /**
   * @brief Prints as many whole bytes as possible to stream.
   * 
   * @remark Aligned to most significant bit, as many as 7 bits can be missed.
   */
  void print( ofstream &stream ) const
  {
    for( int8_t shift = length - 8; shift >= 0; shift -= 8 )
    {
      const uint8_t temp = bits >> shift;
      stream.put( temp );
    }
  };

  /**
   * @brief Shrinks top most bits to length of count.
   * 
   * @param count New desired length.
   */
  void shrink( int8_t count )
  {
    length = count;
    bits &= ones( count );
  };
};

/**
 * @class bitStream
 * 
 * @brief Class for I/O operations with individual bits on stream.
 */
class bitStream : public fstream
{
public:
  bitStream( const string &fileName, ios_base::openmode openFlags )
    : fstream( fileName, openFlags )
  {
    streamState = fstream::rdstate();
  };

  /**
   * @brief Retrieves byte from stream.
   *
   * @remark Needed to be compatible with fstream::get() inside templates.
   * @remark Set's badbit / failbit / eofbit if error occurs.
   *
   * @param length number of requested bits.
   */
  int get()
  {
    return get(8).bits;
  };

  /**
   * @brief Retrieves length of bits from stream.
   *
   * @remark During read, the buffer stays full as much as possible.
   * @remark Set's badbit / failbit / eofbit if error occurs.
   *
   * @param length number of requested bits
   */
  bitArray get( const uint8_t length )
  {
    fillBuffer();

    if( fstream::bad() )
      streamState |= ios_base::iostate::_S_badbit;

    if( length > buffer.length )
    {
      // failed to retrieve requested amount
      streamState |= ios_base::iostate::_S_failbit;

      // lack of bits could be caused by badbit
      // possible combinations are ( bad + fail ) / ( eof + fail )
      if( streamState & ios_base::iostate::_S_eofbit )
        streamState |= ios_base::iostate::_S_eofbit; // we could not retrieve requested amount
      return bitArray(); // invalid bitArray
    }
    return buffer.pop( length );
  };


  /**
   * @brief Writes data to stream.
   *
   * @remark During writing, the buffer stays empty as much as possible.
   * @remark Set's badbit if error occurs.
   */
  void write( const bitArray &data )
  {
    buffer += data;
    while( buffer.length >= 8 ) writeByte();
  };

  /**
   * @brief Padds bits in bitStream with zeros to multiple of byte.
   * 
   * @remark Used to force buffer to get written to underlying stream.
   * @remark Set's badbit if error occurs.
   */
  void flush()
  {
    if( !buffer.length ) return;
    buffer <<= 8 - buffer.length;
    writeByte();
    fstream::flush();

    // badbit could be set by fstream::flush()
    if( fstream::bad() )
      streamState |= ios_base::iostate::_S_badbit;
  };

  bool good()
  {
    return streamState == ios_base::iostate::_S_goodbit;
  };

  bool fail()
  {
    return streamState & ( ios_base::iostate::_S_badbit | ios_base::iostate::_S_failbit );
  };

  bool bad()
  {
    return streamState & ios_base::iostate::_S_badbit;
  };

  bool eof()
  {
    return streamState & ios_base::iostate::_S_eofbit;
  };

private:
  /**
   * @brief Fills internal buffer from underlying stream as much as possible.
   */
  void fillBuffer()
  {
    while( buffer.length + 8 <= BUFFER_SIZE )
    {
      uint8_t newByte = fstream::get();

      if( !fstream::good() )
        return;

      buffer += newByte;
    }
  };

  /**
   * @brief Writes byte to underlying stream.
   * 
   * @remark Byte is smallest supported fstream type.
   * @remark Set's badbit if error occurs.
   */
  void writeByte()
  {
    fstream::put( buffer.pop( 8 ) );
    if( fstream::bad() )
      streamState |= ios_base::iostate::_S_badbit;
  };

  bitArray buffer = bitArray();
  std::ios_base::iostate streamState = std::ios_base::iostate::_S_goodbit;
  const static uint8_t BUFFER_SIZE = 64;
};

/**
 * @class smallHuffmanCode
 * 
 * @brief Class for compression and decompresion of files
 *        using Huffman (en)coding.
 */
class smallHuffmanCode
{
public:
  smallHuffmanCode(){};
  smallHuffmanCode( const smallHuffmanCode & ) = delete;
  smallHuffmanCode &operator=( const smallHuffmanCode & ) = delete;

  ~smallHuffmanCode()
  {
    delete top;
  };

  /**
   * @brief Decompresses file.
   * 
   * @return true on successful decompresion.
   * @return false on corrupted / unreadable file.
   */
  bool decompress( const char *inFileName, const char *outFileName )
  {
    bitStream input( inFileName, ifstream::in | ifstream::binary );
    if( !input.good() ) return false;

    ofstream output( outFileName, fstream::binary | fstream::trunc );
    if( !output.good() ) return false;

    if( !loadTree( input ) ) return false;

    if( !readChunks( input, output ) ) return false;

    // input can fail because of EOF, but can't have io problem
    // output can't fail in no way
    if( input.bad() || output.fail() ) return false;

    return true;
  };

  /**
   * @brief Function for compressing file
   * 
   * @return true on successful compresion
   * @return false on corrupted / unreadable file
   */
  bool compress( const char *inFileName, const char *outFileName )
  {
    ifstream inFile( inFileName, ifstream::binary );
    if( !inFile.good() ) return false;

    if( !createTreeFrom( inFile ) ) return false;

    bitStream output( outFileName, fstream::out | fstream::binary | fstream::trunc );
    if( !output.good() ) return false;

    printHuffCode( output );
    while( printChunk( inFile, output ) );

    output.flush();

    // inFile can fail because of EOF but cant have io problem
    if( inFile.bad() || output.fail() ) return false;
    return true;
  };

private:
  /**
   * @struct node
   * @brief Structure for representing Huffman tree.
   * 
   * @remark 0: left
   *         1: right
   * 
   * @remark left == right == nullptr means that node is leaf and utf8 stores
   *         valid utf8 bytes for series of bits from top to current node
   *         otherwise utf8 should be empty as Huffman code is prefix-free code.
   */
  struct node
  {
    node *left = nullptr;
    node *right = nullptr;
    bitArray utf8;

    node()
    {};

    // leaf node constructor
    node( const bitArray &utf8 )
      : utf8( utf8 )
    {};

    // parent node constructor
    node( node *left, node *right )
      : left( left ), right ( right )
    {};

    node( const node & ) = delete;
    node &operator=( const node & ) = delete; // todo remove 4 lines

    ~node()
    {
      delete right;
      delete left;
    }

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
        return utf8 = getUtf8( input );
      else return false;
    };

    /**
     * @brief Prints Huffman tree to compressed file
     */
  };

/* VVV DECOMPRESSION PRIVATE METHODS VVV */

  /**
   * @brief Loads Huffman tree from file.
   *
   * @return true on success
   * @return false on corrupted UTF-8 character / EOF / IO-error
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
   * @brief Reads Huffman code from input and prints decoded data to output
   *
   * @param chunkSize count of UTF-8 chracters needed at output
   * @return true on succes
   * @return false EOF, IO-error occured before chuckSize characters could be read
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
   * @brief Create a Huffman tree/code to compress input
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
   * @brief Print Huffman code for sub-tree of current node.
   *
   * @param current root of tree
   * @param code Huffman code for root
   */
  void printTree( const node *current, bitArray &code, bitStream &output )
  {
    if( current->left && current->right )
    {
      output.write( bitArray( 0, 1 ) );
      code += bitArray( 0, 1 );
      printTree( current->left, code, output );
      code |= 1;
      printTree( current->right, code, output );
      code.pop();
      return;
    }
    output.write( bitArray( 1, 1 ) );
    output.write( current->utf8 );
    lookUpTable[ current->utf8 ] = code;
  };


  /**
   * @brief Stores UTF-8 character frequencies in input to charFreq.
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
   * @brief Extracts UTF-8 byte representation of character to target
   * 
   * @return true on success
   * @return false on invalid UTF-8 format in stream
   *
   * @remark Invalid UTF-8 format can result from:
   * - lack of bits( EOF )
   * - first byte being continuation byte
   * - lack of continuation bytes
   * - underlying UNICODE represented by UTF-8 out of range
   * 
   * @remark function check for correctness of represented UNICODE but result is still UTF-8
   * 
   * @remark Function is duplicated because we need reading from both stream and bitStream
   * @sa bool bitStream::getUtf8( bitArray &target )
   */
  /*
  static bitArray getUtf8( ifstream &input )
  {
    uint32_t leading = input.get();
    if( !input.good() )
      return bitArray( !input.bad(), 0 );

    uint8_t byteCount = __builtin_clz( ~( leading << 24 ) );
    if( byteCount == 1 || byteCount > 4 )
      return bitArray();

    bitArray utf8( leading, 8 );

    for( uint8_t byte = 1; byte < byteCount; ++byte )
    {
      uint8_t continuation = input.get();
      if( !input.good() ) return bitArray();
      if( ( continuation & ( 0b11 << 6 ) ) != ( 1 << 7 ) ) return bitArray();
      utf8 += continuation;
    }

    if( utf8.bits >= 0xf4908080ul ) return bitArray();
    return utf8;
  };*/

  /**
   * @brief Pop and return smallest element from one of two sorted queues
   *        comparision is made on second item of pair ( occurence ).
   *
   * @remark There has to be at least one element in some queue.
   * 
   * @return pair<node *, uint64_t> pair with smallest .second
   */
  static pair<node *, uint64_t> popSmallest( queue< pair<node *, uint64_t> > &queue_a,
                                             queue< pair<node *, uint64_t> > &queue_b )
  {
    pair<node *, uint64_t> temp;
    if( !queue_a.size() ) // no elements in queue_a
    {
      temp = queue_b.front();
      queue_b.pop();
      return temp;
    }

    if( !queue_b.size() ) // no elements in queue_b
    {
      temp = queue_a.front();
      queue_a.pop();
      return temp;
    }

    // there is at least one element in each queue
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
   * @brief Compair occurences of < node *, int occurence > pair
   *
   * @return  1 if lhs > rhs,
   *         -1 if lhs < rhs,
   *          0 otherwise
   */
  inline static int8_t compPair( const pair<node *, uint64_t> &lhs,
                                 const pair<node *, uint64_t> &rhs )
  {
    return ( lhs.second > rhs.second ) - ( lhs.second < rhs.second );
  };

  /**
   * @brief Create pair < parent node, combined occurence >
   *
   * @param a < child node, occurence >
   * @param b < child node, occurence >
   * @return pair<node *, uint64_t> parent
   */
  static pair<node *, uint64_t> merge( pair<node *, uint64_t> a,
                                       pair<node *, uint64_t> b )
  {
    return pair<node *, uint64_t>( new node( a.first, b.first ), a.second + b.second );
  };

  /**
   * @brief Prints Huffman tree mapping from Huffman code to UTF-8 to output.
   *        Updates lookUpTable to reflect currently stored tree.
   */
  void printHuffCode( bitStream &output )
  {
    if( !top ) return;
    bitArray code;
    printTree( top, code, output );
  };

  /**
   * @brief Extracts < 0, 4096 > characters from inFile,
   *        and stores them in compressed chunk.
   *
   * @remark Chunk consists of either 1< Block of 4096 compressed characters >
   *                               or 0< 12 bit number N >< Block of N compressed characters >.
   *
   * @return true if there is still more data in inFile.
   * @return false on EOF.
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
   * @brief Prints compressed data to output.
   * 
   * @param data Vector of characters to be encoded and printed.
   */
  void printBlock( const vector<bitArray> &data, bitStream &output ) const
  {
    for( const bitArray &symbol: data )
    {
      output.write( lookUpTable.at( symbol ) );
    }
  };

  /**
   * @brief Huffman code  --->  UTF-8
   *        Root of Huffman tree.
   */
  node *top = nullptr;

  /**
   * @brief UTF-8  --->  Huffman code
   * @remark bitArray is limited to length of 32 bits. Also Huffman tree can be highly
   *         unbalanced. For example occurances { 1, 2, 4, 8, ... 2^n } of characters
   *         produces tree with depth n-1, which means that it would be needed n-2 bits
   *         to represent all codes. For n = 35 codes wouldn't fit to bitArray. This could 
   *         happen for file with size ~68.7GB.
   * 
   * @sa For alternative implementation which uses recursion to avoid this problem
   *     see commit 95b12455:"hw01: bitArray simplified/ read ->get, write->put... faster t<1.1s"
   *     in which slower recursion version was last used( 1.5s --> 1.0s speed increase )
   */
  map< bitArray, bitArray > lookUpTable;
};

template < class streamT >
static bitArray getUtf8( streamT &stream )
{
  uint32_t leading = stream.get();
  if( !stream.good() )
    return bitArray( !stream.bad(), 0 );

  int byteCount = __builtin_clz( ~( leading << 24 ) );
  if( byteCount == 1 || byteCount > 4 )
    return bitArray();

  bitArray utf8( leading, 8 );

  for( int byte = 1; byte < byteCount; ++byte )
  {
    uint8_t continuation = stream.get();
    if( !stream.good() ) return bitArray();
    if( ( continuation & ( 0b11 << 6 ) ) != ( 1 << 7 ) ) return bitArray();
    utf8 += continuation;
  }

  if( utf8.bits >= 0xf4908080ul ) return bitArray();
  return utf8;
};



  /**
   * @brief Extracts UTF-8 byte representation of character to target
   * 
   * @return true on success
   * @return false on invalid UTF-8 format in stream
   *
   * @remark Invalid UTF-8 format can result from:
   * - lack of bits( EOF )
   * - first byte being continuation byte
   * - lack of continuation bytes
   * - underlying UNICODE represented by UTF-8 out of range
   * 
   * @remark function check for correctness of represented UNICODE but result is still UTF-8
   * 
   * @sa getUtf8( ifstream &input )
   */
/*
  bool getUtf8( bitArray &target )
  {
    bitArray leading = get( 8 );
    if( !leading ) return false;

    // 0b<3 zero bytes> 1110 XXXX ---> ( << 24 )
    // 0b1110 XXXX <3 zero bytes> ---> ~
    // 0b0001 XXXX <3 ones bytes> ---> clz: number of leading zeros
    uint8_t byteCount = __builtin_clz( ~( leading.bits << 24 ) );

    // 1 means first == 10XX XXXX (continuation byte as first byte)
    if( byteCount == 1 || byteCount > 4 ) return false;

    for( uint8_t byte = 1; byte < byteCount; ++byte )
    {
      bitArray continuation = get( 8 );
      if( continuation.peekFirst( 2 ) != bitArray( 0b10, 2 ) )
        return false;
      leading += continuation;
    }

    *//* 0xf4908080 ==
     * 0b 1111 0100
     *    1001 0000
     *    1000 0000
     *    1000 0000 ==
     * max UTF-8 value + 1
     *//*
    if( leading.bits >= 0xf4908080ul ) return bitArray();
    target = leading;
    return true;
  };*/


bool decompressFile ( const char * inFileName, const char * outFileName )
{
  smallHuffmanCode huff;
  return huff.decompress( inFileName, outFileName );
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  smallHuffmanCode huff;
  return huff.compress( inFileName, outFileName );
}

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  ifstream f1 ( fileName1, ifstream::binary | ifstream::ate );
  ifstream f2 ( fileName2, ifstream::binary | ifstream::ate );

  if( f1.fail() || f2.fail() )
    return false; // file problem

  if( f1.tellg() != f2.tellg() )
    return false; // size mismatch

  // seek back to beginning and use std::equal to compare contents
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
