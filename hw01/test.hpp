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
  return ( ( 1ull << n ) - 1 ) | -( n == 64 );
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
  bitArray();

  /**
   * @brief Construct with value and length.
   * @remark Value set bits higher than length are cleared, except LSB to allow EOF construction.
   */
  bitArray( uint64_t value, uint8_t length );

  /**
   * @remark Validity of operation is guarateed by properties of <
   *
   * @return true if arguments are identical.
   * @return false if arguments differ.
   */
  friend bool operator==( const bitArray &lhs, const bitArray &rhs );

  /**
   * @return true if arguments differ.
   * @return false if arguments are identical.
   */
  friend bool operator!=( const bitArray &lhs, const bitArray &rhs );

  friend bitArray operator&( const bitArray &lhs, uint64_t rhs );

  /**
   * @remark Bits higher than highest bit of array are cleared.
   */
  friend bitArray operator|( const bitArray &lhs, uint64_t rhs );

  bitArray &operator&=( uint64_t rhs );

  /**
   * @remark Bits higher than highest bit of array are cleared.
   */
  bitArray &operator|=( uint64_t rhs );

  /**
   * @param count of bits ( length ).
   * @return bitArray of first bits.
   */
  bitArray peek( uint8_t count = 1 ) const;

  /**
   * @brief Appends/Concatenates other bitArray.
   */
  bitArray &operator+=( const bitArray &other );

  /**
   * @brief Appends/Concatenates 8 bits.
   */
  bitArray &operator+=( const uint8_t other );

  /**
   * @brief Concatenates bitArrays.
   */
  friend bitArray operator+( const bitArray &lhs, const bitArray &rhs );

  bitArray &operator<<=( int shift );

  friend bitArray operator<<( const bitArray &lhs , int shift );

  /**
   * @brief Unspecified asymetric transitive relation.
   * 
   * @remark Oparation introduced for use in algorithms / data structures which needs
   * sorting / comparision operator.
   */
  friend bool operator<( const bitArray &lhs, const bitArray &rhs );

  /**
   * @brief Unspecified asymetric transitive relation.
   */
  friend bool operator>( const bitArray &lhs, const bitArray &rhs );

  /**
   * @return true for valid bitArrays.
   * @return false for empty bitArray / EOF.
   */
  operator bool() const;

  // more operator could be easily implemented which would make sense ~, >=, <=, maybe []
  // ...but I only needed those up there and some of them are not even used

  /**
   * @return bitArray of extracted bits from front.
   */
  bitArray pop( uint8_t count = 1 );

  /**
   * @brief Prints as many whole bytes as possible to stream.
   * 
   * @remark Aligned to most significant bit, as many as 7 bits can be missed.
   */
  void print( ofstream &stream ) const;

  /**
   * @brief Shrinks top most bits to length of count.
   * 
   * @param count New desired length.
   */
  void shrink( int8_t count );
};

/**
 * @class bitStream
 * 
 * @brief Class for I/O operations with individual bits on stream.
 */
class bitStream : public fstream
{
public:
  bitStream( const string &fileName, ios_base::openmode openFlags );

  /**
   * @brief Retrieves byte from stream.
   *
   * @remark Needed to be compatible with fstream::get() inside templates.
   * @remark Set's badbit / failbit / eofbit if error occurs.
   *
   * @param length number of requested bits.
   */
  int get();

  /**
   * @brief Retrieves length of bits from stream.
   *
   * @remark During read, the buffer stays full as much as possible.
   * @remark Set's badbit / failbit / eofbit if error occurs.
   *
   * @param length number of requested bits
   */
  bitArray get( const uint8_t length );


  /**
   * @brief Writes data to stream.
   *
   * @remark During writing, the buffer stays empty as much as possible.
   * @remark Set's badbit if error occurs.
   */
  void write( const bitArray &data );

  /**
   * @brief Padds bits in bitStream with zeros to multiple of byte.
   * 
   * @remark Used to force buffer to get written to underlying stream.
   * @remark Set's badbit if error occurs.
   */
  void flush();

  bool good();

  bool fail();

  bool bad();

  bool eof();

private:
  /**
   * @brief Fills internal buffer from underlying stream as much as possible.
   */
  void fillBuffer();

  /**
   * @brief Writes byte to underlying stream.
   * 
   * @remark Byte is smallest supported fstream type.
   * @remark Set's badbit if error occurs.
   */
  void writeByte();

  bitArray buffer = bitArray();
  std::ios_base::iostate streamState = std::ios_base::iostate::_S_goodbit;
  const static uint8_t BUFFER_SIZE = 64;
};


/**
 * @brief Extracts UTF-8 byte representation of UNICODE character.
 * 
 * @return bitArray of UTF-8 code or error bitArray.
 *
 * @remark Invalid UTF-8 format can result from:
 *           - lack of bits( EOF )
 *           - first byte being continuation byte
 *           - lack of continuation bytes
 *           - underlying UNICODE represented by UTF-8 out of range
 * 
 * @remark Function is templated to support reading from regular fstream and bitStream.
 */
template < class streamT >
static bitArray getUtf8( streamT &stream );


/**
 * @class smallHuffmanCode
 * 
 * @brief Class for compression and decompression of files
 *        using Huffman (en)coding.
 */
class smallHuffmanCode
{
public:
  smallHuffmanCode();
  smallHuffmanCode( const smallHuffmanCode & ) = delete;
  smallHuffmanCode &operator=( const smallHuffmanCode & ) = delete;

  /**
   * @brief Decompresses file.
   * 
   * @return true on successful decompression.
   * @return false on corrupted / unreadable file.
   */
  bool decompress( const char *inFileName, const char *outFileName );

  /**
   * @brief Function for compressing file.
   * 
   * @return true on successful compression.
   * @return false on corrupted / unreadable file.
   */
  bool compress( const char *inFileName, const char *outFileName );

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
    unique_ptr<node> left;
    unique_ptr<node> right;
    bitArray utf8;

    node();

    // leaf node constructor
    node( const bitArray &utf8 );

    // parent node constructor
    node( node *left, node *right );

    node( const node & ) = delete;
    node &operator=( const node & ) = delete;

    /**
     * @brief Creates subtree from this node.
     *
     * @return true on success.
     * @return false on corrupted UTF-8 characted / EOF / IO-error.
     */
    bool appendNode( bitStream &input );
  };

/* VVV DECOMPRESSION PRIVATE METHODS VVV */

  /**
   * @brief Loads Huffman tree from file.
   *
   * @return true on success.
   * @return false on corrupted UTF-8 character / EOF / IO-error.
   */
  bool loadTree( bitStream &input );

  /**
   * @brief Read chunks from input and decodes them.
   * 
   * @return true on success.
   * @return false on bad / corrupted input format.
   */
  bool readChunks( bitStream &input, ofstream &output ) const;

  /**
   * @brief Reads Huffman code from input and prints decoded data to output.
   *
   * @param chunkSize count of UTF-8 chracters needed at output.
   * @return true on success.
   * @return false if EOF / IO-error occured before chuckSize characters could be read.
   */
  bool readChunk( bitStream &input, ofstream &output, uint16_t chunkSize = 4096 ) const;

/* VVV COMPRESSION PRIVATE METHODS VVV */

  /**
   * @brief Create a Huffman tree/code to compress input.
   *
   * @return true on success.
   * @return false on bad UTF-8 coding or IO-error.
   */
  bool createTreeFrom( ifstream &input );

  /**
   * @brief Calculates occurance of UTF-8 characters in input.
   *
   * @param[out] charFreq map for storing <UTF-8, count> pairs.
   * @return true on success.
   * @return false on bad UTF-8 coding or IO-error.
   */
  static bool getCharacterCounts( ifstream &input, map<bitArray, uint64_t> &charFreq );

  /**
   * @brief Pop and return smallest element from one of two sorted queues
   *        comparision is made on second item of pair ( occurence ).
   *
   * @remark There has to be at least one element in some queue.
   * 
   * @return pair<node *, uint64_t> pair with smallest .second
   */
  static pair<node *, uint64_t> popSmallest( queue< pair<node *, uint64_t> > &queue_a,
                                             queue< pair<node *, uint64_t> > &queue_b );

  /**
   * @brief Compair occurences of < node *, int occurence > pair
   *
   * @return  1 if lhs > rhs,
   *         -1 if lhs < rhs,
   *          0 otherwise
   */
  static int8_t compPair( const pair<node *, uint64_t> &lhs,
                          const pair<node *, uint64_t> &rhs );

  /**
   * @brief Create pair < parent node, combined occurence >
   *
   * @param a < child node, occurence >
   * @param b < child node, occurence >
   * @return pair<node *, uint64_t> parent
   */
  static pair<node *, uint64_t> merge( pair<node *, uint64_t> a,
                                       pair<node *, uint64_t> b );

  /**
   * @brief Prints Huffman tree mapping from Huffman code to UTF-8 to output.
   *        Updates lookUpTable to reflect currently stored tree.
   */
  void printHuffCode( bitStream &output );

  /**
   * @brief Print Huffman code for sub-tree of current node.
   *
   * @param current root of sub-tree.
   * @param code Huffman code for current node
   */
  void printTree( const node *current, bitArray &code, bitStream &output );

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
  bool printChunk( ifstream &inFile, bitStream &output ) const;

  /**
   * @brief Prints compressed data to output.
   * 
   * @param data Vector of characters to be encoded and printed.
   */
  void printBlock( const vector<bitArray> &data, bitStream &output ) const;

  /**
   * @brief Huffman code  --->  UTF-8
   *        Root of Huffman tree.
   */
  unique_ptr<node> top;

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
  map<bitArray, bitArray> lookUpTable;
};


bool decompressFile( const char * inFileName, const char * outFileName );

bool compressFile( const char * inFileName, const char * outFileName );
