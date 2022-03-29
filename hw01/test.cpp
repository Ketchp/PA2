#include "test.hpp"

ostream &operator<<( ostream &os, bitArray arr )
{
  while( arr )
  {
    os << arr.pop().bits;
  }
  return os;
}

/* VVV bitArray definitions VVV */

bitArray::bitArray()
{}

bitArray::bitArray( uint64_t value, uint8_t length )
  : bits( value & ( ones( length ) | 1 ) ), length( length )
{}

bool operator==( const bitArray &lhs, const bitArray &rhs )
{
  return lhs.bits == rhs.bits && lhs.length == rhs.length;
}

bool operator!=( const bitArray &lhs, const bitArray &rhs )
{
  return !( lhs == rhs );
}

bitArray operator&( const bitArray &lhs, uint64_t rhs )
{
  return bitArray( lhs.bits & rhs, lhs.length );
}

bitArray operator|( const bitArray &lhs, uint64_t rhs )
{
  return bitArray( lhs.bits | ( rhs & ones( lhs.length ) ), lhs.length );
}

bitArray &bitArray::operator&=( uint64_t rhs )
{
  return *this = *this & rhs;
}

bitArray &bitArray::operator|=( uint64_t rhs )
{
  return *this = *this | rhs;
}

bitArray bitArray::peek( uint8_t count ) const
{
  return bitArray( bits >> ( length - count ), count );
}

bitArray &bitArray::operator+=( const bitArray &other )
{
  return *this = *this + other;
}

bitArray &bitArray::operator+=( const uint8_t other )
{
  return *this = *this + bitArray( other, 8 );
}

bitArray operator+( const bitArray &lhs, const bitArray &rhs )
{
  return ( lhs << rhs.length ) | rhs.bits;
}

bitArray &bitArray::operator<<=( int shift )
{
  return *this = *this << shift;
}

bitArray operator<<( const bitArray &lhs , int shift )
{
  return bitArray( lhs.bits << shift, lhs.length + shift );
}

bool operator<( const bitArray &lhs, const bitArray &rhs )
{
  return lhs.bits == rhs.bits ? lhs.length < rhs.length : lhs.bits < rhs.bits;
}

bool operator>( const bitArray &lhs, const bitArray &rhs )
{
  return rhs < lhs;
}

bitArray::operator bool() const
{
  return length;
}

bitArray bitArray::pop( uint8_t count )
{
  bitArray result( *this );
  shrink( length - count );
  return result.peek( count );
}

void bitArray::print( ofstream &stream ) const
{
  for( int8_t shift = length - 8; shift >= 0; shift -= 8 )
  {
    const uint8_t temp = bits >> shift;
    stream.put( temp );
  }
}

void bitArray::shrink( int8_t count )
{
  length = count;
  bits &= ones( count );
}

/* VVV bitStream definitions VVV */

bitStream::bitStream( const string &fileName, ios_base::openmode openFlags )
  : fstream( fileName, openFlags )
{
  streamState = fstream::rdstate();
}

int bitStream::get()
{
  return get(8).bits;
}

bitArray bitStream::get( const uint8_t length )
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
}

void bitStream::write( const bitArray &data )
{
  buffer += data;
  while( buffer.length >= 8 ) writeByte();
}

void bitStream::flush()
{
  cout << (int)buffer.length << endl;
  if( !buffer.length ) return;
  buffer <<= 8 - buffer.length;
  writeByte();
  fstream::flush();

  // badbit could be set by fstream::flush()
  if( fstream::bad() )
    streamState |= ios_base::iostate::_S_badbit;
}

bool bitStream::good()
{
  return streamState == ios_base::iostate::_S_goodbit;
}

bool bitStream::fail()
{
  return streamState & ( ios_base::iostate::_S_badbit | ios_base::iostate::_S_failbit );
}

bool bitStream::bad()
{
  return streamState & ios_base::iostate::_S_badbit;
}

bool bitStream::eof()
{
  return streamState & ios_base::iostate::_S_eofbit;
}

void bitStream::fillBuffer()
{
  while( buffer.length + 8 <= BUFFER_SIZE )
  {
    uint8_t newByte = fstream::get();

    if( !fstream::good() )
      return;

    buffer += newByte;
  }
}

void bitStream::writeByte()
{
  fstream::put( buffer.pop( 8 ).bits );
  if( fstream::bad() )
    streamState |= ios_base::iostate::_S_badbit;
}

template < class streamT >
static bitArray getUtf8( streamT &stream )
{
  uint32_t leading = stream.get();
  if( !stream.good() )
    // stream is empty / bad
    return bitArray( !stream.bad(), 0 );

  // 0b<3 zero bytes> 1110 XXXX ---> ( << 24 )
  // 0b1110 XXXX <3 zero bytes> ---> ~
  // 0b0001 XXXX <3 ones bytes> ---> clz: count leading zeros
  int byteCount = __builtin_clz( ~( leading << 24 ) );

  if( byteCount == 1 || byteCount > 4 )
    // first byte beeing continuation or unsuported count continuation bytes
    return bitArray();

  bitArray utf8( leading, 8 );

  for( int byte = 1; byte < byteCount; ++byte )
  {
    uint8_t continuation = stream.get();
    if( !stream.good() )
      // stream is empty / bad
      return bitArray();

    if( ( continuation & ( 0b11 << 6 ) ) != ( 1 << 7 ) )
      // first byte is not continuation byte
      return bitArray();

    utf8 += continuation;
  }

  /*  biggest character which can be coded using UTF-8: 0x 10FFFF
   *  in binary 0b 1 0000
   *            1111 1111
   *            1111 1111
   *
   *  coded in UTF-8 0b 11110 100 in hex: 0x f4
   *                    10 001111            8f
   *                    10 111111            bf
   *                    10 111111            bf
   *
   *  because UTF-8 coding bits are same for all 4-byte characters
   *  we can compare UTF-8 representations instead of represented values
   *  eg. 0xf48fbfbf instead of 0x10FFFF
   */
  static unsigned long MAX_UTF_8 = 0xf48fbfbful;
  if( utf8.bits > MAX_UTF_8 ) return bitArray();
  return utf8;
}

/* VVV smallHuffmanCode::node definitions VVV */

smallHuffmanCode::node::node()
{}

smallHuffmanCode::node::node( const bitArray &utf8 )
  : utf8( utf8 )
{}

smallHuffmanCode::node::node( node *left, node *right )
  : left( left ), right ( right )
{}

bool smallHuffmanCode::node::appendNode( bitStream &input )
{
  bitArray first = input.get( 1 );
  if( first == bitArray( 0, 1 ) )
  {
    left = make_unique<node>();
    right = make_unique<node>();
    return left->appendNode( input ) && right->appendNode( input );
  }
  else if( first == bitArray( 1, 1 ) )
    return utf8 = getUtf8( input );
  else return false;
}


/* VVV smallHuffmanCode definitions VVV */

smallHuffmanCode::smallHuffmanCode()
{}

bool smallHuffmanCode::decompress( const char *inFileName, const char *outFileName )
{
  bitStream input( inFileName, ifstream::in | ifstream::binary );
  if( !input.good() ) return false;

  ofstream output( outFileName, fstream::binary | fstream::trunc );
  if( !output.good() ) return false;

  if( !loadTree( input ) ) return false;

  bitStream temp( "garbage", fstream::out | fstream::binary | fstream::trunc );
  printHuffCode( temp );

  if( !readChunks( input, output ) ) return false;

  // input can fail because of EOF, but can't have IO problem
  // output can't fail in no way
  return !input.bad() && !output.fail();
}

bool smallHuffmanCode::compress( const char *inFileName, const char *outFileName )
{
  ifstream inFile( inFileName, ifstream::binary );
  if( !inFile.good() ) return false;

  if( !createTreeFrom( inFile ) ) return false;

  bitStream output( outFileName, fstream::out | fstream::binary | fstream::trunc );
  if( !output.good() ) return false;

  printHuffCode( output );

  while( printChunk( inFile, output ) );

  output.flush();

  // inFile can fail because of EOF but can't have IO problem
  return !inFile.bad() && !output.fail();
}

/* VVV DECOMPRESSION PRIVATE METHODS VVV */

bool smallHuffmanCode::loadTree( bitStream &input )
{
  top = make_unique<node>();
  return top->appendNode( input );
}

bool smallHuffmanCode::readChunks( bitStream &input, ofstream &output ) const
{
  bitArray chunkType = input.get( 1 );

  // Chunk starting 0b1 means 4096 compressed UTF-8 chars follows.
  while( chunkType == bitArray( 1, 1 ) )
  {
    if( !readChunk( input, output ) ) return false;
    chunkType = input.get( 1 );
  }

  if( chunkType != bitArray( 0, 1 ) )
    return false;

  // Chunk starting 0b0 is followed by 12bits representing count of UTF-8 chars.
  bitArray chunkSize = input.get( 12 );
  if( !chunkSize ) return false;
  if( !readChunk( input, output, chunkSize.bits ) ) return false;

  // Input file can't contain one whole useless byte.
  return input.get( 8 ) == bitArray();
}

bool smallHuffmanCode::readChunk( bitStream &input, ofstream &output, uint16_t chunkSize ) const
{
  while( chunkSize )
  {
    node *current = top.get();
    while( current->right || current->left )
    {
      bitArray dir = input.get( 1 );
      if( dir == bitArray( 0, 1 ) )
        current = current->left.get();
      else if( dir == bitArray( 1, 1 ) )
        current = current->right.get();
      else return false;
    }
    current->utf8.print( output );
    --chunkSize;
  }
  return true;
}

/* VVV COMPRESSION PRIVATE METHODS VVV */

bool smallHuffmanCode::createTreeFrom( ifstream &input )
{
  map<bitArray, uint64_t> characterCounts;
  if( !getCharacterCounts( input, characterCounts ) )
    return false;

  // map can't be sorted
  vector< pair<node *, uint64_t> > charArray;
  charArray.reserve( characterCounts.size() );

  for( const pair<bitArray, uint64_t> &character: characterCounts )
    charArray.push_back( pair<node *, uint64_t>( new node( character.first ),
                                                  character.second ) );

  sort( charArray.begin(), charArray.end(),
        []( const pair<node *, uint64_t> &a, const pair<node *, uint64_t> &b )
        {
            return a.second < b.second;
        } );

  /* From ordered queue we want to merge to single node.
    * We need to find two smallest elements fast so queue needs to be always ordered.
    * Removal of those elements and insertion of merged elements also needs to be fast.
    * Ideal data structure for this is linked list.
    * Lets start with two ordered queues source, target.
    * We can find two smallest elements in first two places of queues.
    * We always pick smallest elements so each new merged element is greater or equal than previous.
    * Inserting to end of target will maintain order.
    * After some steps source will become empty,
    * then after n - 1 steps there will be only single element in target.
    */
  //queue< pair<node *, uint64_t> > source( charArray.cbegin(), charArray.cend() ); // C++23
  queue< pair<node *, uint64_t> > source, target;
  for( const pair<node *, uint64_t> &letter: charArray )
    source.push( letter );

  // iterate until we have only one element in target ( none in source )
  while( target.size() > 1 || source.size() )
    target.push( merge( popSmallest( source, target ),
                        popSmallest( source, target ) ) );

  top = unique_ptr<node>( target.front().first );
  return true;
}

bool smallHuffmanCode::getCharacterCounts( ifstream &input, map<bitArray, uint64_t> &charFreq )
{
  while( true )
  {
    bitArray symbol = getUtf8( input );
    if( !symbol )
    {
      // set stream to starting position
      input.clear();
      input.seekg( 0 );
      // check if EOF / bad stream happened
      return symbol == bitArray( 1, 0 );
    }
    charFreq[ symbol ]++;
  }
}

pair<smallHuffmanCode::node *, uint64_t>
smallHuffmanCode::popSmallest( queue< pair<smallHuffmanCode::node *, uint64_t> > &queue_a,
                               queue< pair<smallHuffmanCode::node *, uint64_t> > &queue_b )
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
}

int8_t smallHuffmanCode::compPair( const pair<node *, uint64_t> &lhs,
                                const pair<node *, uint64_t> &rhs )
{
  return ( lhs.second > rhs.second ) - ( lhs.second < rhs.second );
}

pair<smallHuffmanCode::node *, uint64_t>
smallHuffmanCode::merge( pair<smallHuffmanCode::node *, uint64_t> a,
                         pair<smallHuffmanCode::node *, uint64_t> b )
{
  return pair<node *, uint64_t>( new node( a.first, b.first ), a.second + b.second );
}

void smallHuffmanCode::printHuffCode( bitStream &output )
{
  if( !top ) return;
  // code to reach certain node in Huffman tree
  bitArray code;
  printTree( top.get(), code, output );
}

void smallHuffmanCode::printTree( const node *current, bitArray &code, bitStream &output )
{
  if( current->left && current->right )
  {
    // node with child nodes
    output.write( bitArray( 0, 1 ) );

    // left node
    code += bitArray( 0, 1 );
    printTree( current->left.get(), code, output );

    // right node
    code |= 1;
    printTree( current->right.get(), code, output );

    code.pop();
    return;
  }
  // leaf node
  output.write( bitArray( 1, 1 ) );
  output.write( current->utf8 );
  lookUpTable[ current->utf8 ] = code; // updates look-up for character
  cout << (char)current->utf8.bits << ' ' << current->utf8 << ' ' << code << endl;
}

bool smallHuffmanCode::printChunk( ifstream &inFile, bitStream &output ) const
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
}

void smallHuffmanCode::printBlock( const vector<bitArray> &data, bitStream &output ) const
{
  for( const bitArray &symbol: data )
  {
    output.write( lookUpTable.at( symbol ) );
  }
}


bool decompressFile ( const char * inFileName, const char * outFileName )
{
  smallHuffmanCode huff;
  cout << endl << inFileName << " -> " << outFileName << endl;
  return huff.decompress( inFileName, outFileName );
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  smallHuffmanCode huff;
  cout << endl << inFileName << " -> " << outFileName << endl;
  return huff.compress( inFileName, outFileName );
}
