#include "jsonParser.hpp"

jsonValue::jsonValue( jsonType type )
  : m_type( type )
{}

jsonObject::jsonObject()
  : jsonValue( jsonType::jsonObjectType )
{}

jsonArray::jsonArray()
  : jsonValue( jsonType::jsonArrayType )
{}

jsonString::jsonString( std::string value )
  : jsonValue( jsonType::jsonStringType ),
    m_string( move( value ) )
{}

jsonNumber::jsonNumber( std::string number )
  : jsonValue( jsonType::jsonNumberType ),
    m_number( move( number ) )
{}

jsonBool::jsonBool( bool value )
  : jsonValue( jsonType::jsonBoolType ),
    m_bool( value )
{}

jsonNull::jsonNull()
  : jsonValue( jsonType::jsonNullType )
{}

jsonValue *jsonObject::clone() const
{
  return new jsonObject( *this );
}

jsonValue *jsonArray::clone() const
{
  return new jsonArray( *this );
}

jsonValue *jsonString::clone() const
{
  return new jsonString( *this );
}

jsonValue *jsonNumber::clone() const
{
  return new jsonNumber( *this );
}

jsonValue *jsonBool::clone() const
{
  return new jsonBool( *this );
}

jsonValue *jsonNull::clone() const
{
  return new jsonNull( *this );
}

std::set<std::string> jsonValue::getKeys() const
{
  throw std::invalid_argument( "Wrong json type for method getKeys." );
}

size_t jsonValue::size() const
{
  throw std::invalid_argument( "Wrong json type for method size." );
}

jsonValue &jsonValue::operator[]( const std::string & )
{
  throw std::invalid_argument( "Wrong json type for operator[ const std::string & ]." );
}

const jsonValue &jsonValue::operator[]( const std::string & ) const
{
  throw std::invalid_argument( "Wrong json type for operator[ const std::string & ]." );
}

jsonValue &jsonValue::operator[]( size_t )
{
  throw std::invalid_argument( "Wrong json type for operator[ size_t ]." );
}

const jsonValue &jsonValue::operator[]( size_t ) const
{
  throw std::invalid_argument( "Wrong json type for operator[ size_t ]." );
}

jsonValue::operator std::string() const
{
  throw std::invalid_argument( "Invalid json type for string cast." );
}

jsonValue::operator int() const
{
  throw std::invalid_argument( "Invalid json type for int cast." );
}

jsonValue::operator float() const
{
  throw std::invalid_argument( "Invalid json type for float cast." );
}

jsonValue::operator double() const
{
  throw std::invalid_argument( "Invalid json type for double cast." );
}

jsonValue::operator bool() const
{
  throw std::invalid_argument( "Invalid json type for bool cast." );
}


jsonValue *jsonValue::parseFromString( const std::string &data, size_t &position )
{
  parseWhitespace( data, position );
  if( data.size() == position )
    throw std::invalid_argument( "Expected JSON value but got nothing." );
  jsonValue *parsedValue;
  if( data[ position ] == '{' )
    parsedValue = jsonObject::parseFromString( data, position );

  else if( data[ position ] == '[' )
    parsedValue = jsonArray::parseFromString( data, position );

  else if( data[ position ] == '"' )
    parsedValue = jsonString::parseFromString( data, position );

  else if( data[ position ] == '-' || ( '0' <= data[ position ] && data[ position ] <= '9' ) )
    parsedValue = jsonNumber::parseFromString( data, position );

  else if( data[ position ] == 't' || data[ position ] == 'f' )
    parsedValue = jsonBool::parseFromString( data, position );

  else if( data[ position ] == 'n' )
    parsedValue = jsonNull::parseFromString( data, position );
  else
    throw std::invalid_argument( "Expected json value at position " +
                                  std::to_string( position ) );
  parseWhitespace( data, position );
  return parsedValue;
}

void jsonValue::parseWhitespace( const std::string &data, size_t &position )
{
  while( position < data.size() && isspace( data[ position ] ) )
    ++position;
}

jsonValue *jsonObject::parseFromString( const std::string &data, size_t &position )
{
  jsonObject *parsedObject = new jsonObject;
  if( data[ position ] != '{' )
    throw std::invalid_argument( "Expected '{' at position " +
                                  std::to_string( position ) );
  ++position; // '{'

  parseWhitespace( data, position );
  if( data.size() == position )
    throw std::invalid_argument( "Expected '}' or string at end of file." );

  if( data[ position ] == '}' )
  {
    ++position;
    return parsedObject;
  }

  while( true )
  {
    parseWhitespace( data, position );
    if( data.size() == position )
      throw std::invalid_argument( "Expected string at the end of file." );

    jsonValue *parsedString = jsonString::parseFromString( data, position );
    std::string key = (std::string)dynamic_cast<jsonString &>( *parsedString );
    delete parsedString;

    parseWhitespace( data, position );
    if( position == data.size() || data[ position ] != ':' )
      throw std::invalid_argument( "Expected ':' at position " +
                                    std::to_string( position ) );
    ++position; // ':'

    jsonValue *parsedValue = jsonValue::parseFromString( data, position );
    parsedObject->m_object.emplace( key, parsedValue );

    if( data.size() == position )
      throw std::invalid_argument( "Expected ',' or '}' at the end of file." );

    if( data[ position ] == '}' )
      break;
    if( data[ position ] != ',' )
      throw std::invalid_argument( "Unexpected character " +
                                    std::to_string( data[ position ] ) +
                                   " at position: " +
                                    std::to_string( position ) );
    ++position;
  }
  ++position;
  return parsedObject;
}

size_t jsonObject::size() const
{
  return m_object.size();
}

std::set<std::string> jsonObject::getKeys() const
{
  std::set<std::string> keys;
  for( const auto &[ key, _ ]: m_object )
    keys.emplace( key );
  return keys;
}

jsonValue &jsonObject::operator[]( const std::string &key )
{
  return *m_object.at( key );
}

const jsonValue &jsonObject::operator[]( const std::string &key ) const
{
  return *m_object.at( key );
}

jsonValue *jsonArray::parseFromString( const std::string &data, size_t &position )
{
  jsonArray *parsedArray = new jsonArray;
  if( data[ position ] != '[' )
    throw std::invalid_argument( "Expected '[' at position " +
                                 std::to_string( position ) );
  ++position; // '['

  parseWhitespace( data, position );
  if( data.size() == position )
    throw std::invalid_argument( "Expected ']' or value at end of file." );

  if( data[ position ] == ']' )
  {
    ++position;
    return parsedArray;
  }

  while( true )
  {
    jsonValue *parsedValue = jsonValue::parseFromString( data, position );
    parsedArray->m_vector.emplace_back( parsedValue );

    if( data.size() == position )
      throw std::invalid_argument( "Expected ',' or ']' at the end of file." );

    if( data[ position ] == ']' )
      break;
    if( data[ position ] != ',' )
      throw std::invalid_argument( "Unexpected character " +
                                   std::to_string( data[ position ] ) +
                                   " at position: " +
                                   std::to_string( position ) );
    ++position;
  }
  ++position;
  return parsedArray;
}

size_t jsonArray::size() const
{
  return m_vector.size();
}

jsonValue &jsonArray::operator[]( size_t idx )
{
  if( m_vector.size() <= idx )
    throw std::invalid_argument( "Idx out of range." );
  return *m_vector[ idx ];
}

const jsonValue &jsonArray::operator[]( size_t idx ) const
{
  if( m_vector.size() >= idx )
    throw std::invalid_argument( "Idx out of range." );
  return *m_vector[ idx ];
}

jsonValue *jsonString::parseFromString( const std::string &data, size_t &position )
{
  if( data[ position ] != '"' )
    throw std::invalid_argument( "Expected '\"' at position " +
                                 std::to_string( position ) );
  ++position; // '"'

  std::string value;
  while( data.size() > position && data[ position ] != '"' )
  {
    if( iscntrl( data[ position ] ) )
      throw std::invalid_argument( "Control character inside string at position: " +
                                    std::to_string( position ) );
    if( data[ position ] != '\\' )
    {
      value += data[ position ];
      ++position;
      continue;
    }

    ++position; // '\\'
    if( data.size() == position )
      throw std::invalid_argument( "Expected character after \\." );
    switch( data[ position ] )
    {
    case '"':
    case '\\':
    case '/':
      value += data[ position ];
      break;
    case 'b':
      value += '\b';
      break;
    case 'f':
      value += '\f';
      break;
    case 'n':
      value += '\n';
      break;
    case 'r':
      value += '\r';
      break;
    case 't':
      value += '\t';
      break;
    case 'u':
      throw std::invalid_argument( "You are welcome to implement UTF-8." );
    default:
      throw std::invalid_argument( "Unexpected escape character after \\." );
    }
    ++position;
  }

  if( data.size() == position || data[ position ] != '"' )
    throw std::invalid_argument( "Expected '\"' at position " +
                                 std::to_string( position ) );
  ++position; // '"'

  return new jsonString( value );
}

jsonValue *jsonNumber::parseFromString( const std::string &data, size_t &position )
{
  std::string number;
  if( data[ position ] == '-' )
  {
    number += '-';
    position++;
  }
  if( data.size() == position )
    throw std::invalid_argument( "Expected number at position " +
                                  std::to_string( position ) );
  if( data[ position ] == '0' )
    number += data[ position++ ];
  else if( '1' <= data[ position ] && data[ position ] <= '9' )
    while( data.size() != position && '0' <= data[ position ] && data[ position ] <= '9' )
      number += data[ position++ ];
  else
    throw std::invalid_argument( "Expected number at position " +
                                 std::to_string( position ) );

  if( data.size() == position )
    return new jsonNumber( number );

  if( data[ position ] == '.' )
    do number += data[ position++ ];
    while( data.size() != position && '0' <= data[ position ] && data[ position ] <= '9' );

  if( data.size() == position )
    return new jsonNumber( number );

  if( data[ position ] == 'e' || data[ position ] == 'E' )
  {
    number += data[ position++ ];
    if( data.size() == position )
      throw std::invalid_argument( "Expected number after E/e at position " +
                                   std::to_string( position ) );
    if( data[ position ] == '+' || data[ position ] == '-' )
      number += data[ position++ ];
    if( data.size() == position )
      throw std::invalid_argument( "Expected number after E/e at position " +
                                   std::to_string( position ) );
    while( data.size() != position && '0' <= data[ position ] && data[ position ] <= '9' )
      number += data[ position++ ];
  }
  return new jsonNumber( number );
}

jsonValue *jsonBool::parseFromString( const std::string &data, size_t &position )
{
  if( data.size() >= position + 4 &&
      data[ position ] == 't' &&
      data[ position + 1 ] == 'r' &&
      data[ position + 2 ] == 'u' &&
      data[ position + 3 ] == 'e' )
    return new jsonBool( true );
  else if( data.size() >= position + 5 &&
           data[ position ] == 'f' &&
           data[ position + 1 ] == 'a' &&
           data[ position + 2 ] == 'l' &&
           data[ position + 3 ] == 's' &&
           data[ position + 4 ] == 'e' )
    return new jsonBool( false );
  else
    throw std::invalid_argument( "Unexpected character in boolean value at position " +
                                  std::to_string( position ) );
}

jsonValue *jsonNull::parseFromString( const std::string &data, size_t &position )
{
  if( data.size() >= position + 4 &&
      data[ position ] == 'n' &&
      data[ position + 1 ] == 'u' &&
      data[ position + 2 ] == 'u' &&
      data[ position + 3 ] == 'l' )
    return new jsonNull();
  else
    throw std::invalid_argument( "Unexpected character in null value at position " +
                                 std::to_string( position ) );
}

JSON::JSON( const std::string &fileName )
{
  std::ifstream file( fileName );
  if( !file.good() )
    throw std::invalid_argument( "File " + fileName + " could not be opened." );
  size_t position = 0;
  m_top = jsonValue::parseFromString( std::string( std::istreambuf_iterator<char>(file),
                                                   std::istreambuf_iterator<char>() ),
                                      position );
  m_type = m_top->m_type;
  file.seekg( 0, std::ios::end );
  if( position == (size_t)file.tellg() )
    return;
  delete m_top;
  throw std::invalid_argument( "Expected end of file." );
}

JSON::~JSON()
{
  delete m_top;
}