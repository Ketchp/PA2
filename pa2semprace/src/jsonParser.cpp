#include "jsonParser.hpp"


using namespace std;

CJsonValue::CJsonValue( EJsonType type )
        : m_type( type ){}

CJsonObject::CJsonObject()
        : CJsonValue( EJsonType::jsonObjectType ){}

CJsonArray::CJsonArray()
        : CJsonValue( EJsonType::jsonArrayType ){}

CJsonString::CJsonString( string value )
        : CJsonValue( EJsonType::jsonStringType ),
          m_string( move( value ) ){}

CJsonNumber::CJsonNumber( string number )
        : CJsonValue( EJsonType::jsonNumberType ),
          m_number( move( number ) ){}

CJsonBool::CJsonBool( bool value )
        : CJsonValue( EJsonType::jsonBoolType ),
          m_bool( value ){}

CJsonNull::CJsonNull()
        : CJsonValue( EJsonType::jsonNullType ){}

CJsonValue *CJsonObject::clone() const
{
  return new CJsonObject( *this );
}

CJsonValue *CJsonArray::clone() const
{
  return new CJsonArray( *this );
}

CJsonValue *CJsonString::clone() const
{
  return new CJsonString( *this );
}

CJsonValue *CJsonNumber::clone() const
{
  return new CJsonNumber( *this );
}

CJsonValue *CJsonBool::clone() const
{
  return new CJsonBool( *this );
}

CJsonValue *CJsonNull::clone() const
{
  return new CJsonNull( *this );
}

const CJsonObject &CJsonValue::getObject() const
{
  return dynamic_cast<const CJsonObject &>( *this );
}

const CJsonArray &CJsonValue::getArray() const
{
  return dynamic_cast<const CJsonArray &>( *this );
}

const CJsonString &CJsonValue::getJsonString() const
{
  return dynamic_cast<const CJsonString &>( *this );
}

const CJsonNumber &CJsonValue::getJsonNumber() const
{
  return dynamic_cast<const CJsonNumber &>( *this );
}

const CJsonBool &CJsonValue::getJsonBool() const
{
  return dynamic_cast<const CJsonBool &>( *this );
}

const CJsonNull &CJsonValue::getJsonNull() const
{
  return dynamic_cast<const CJsonNull &>( *this );
}

const string &CJsonValue::toString() const
{
  throw invalid_argument( "Wrong type for string conversion." );
}

int CJsonValue::toInt() const
{
  throw invalid_argument( "Wrong type for int conversion." );
}

double CJsonValue::toDouble() const
{
  throw invalid_argument( "Wrong type for real conversion." );
}

size_t CJsonValue::count( const string & ) const
{
  throw invalid_argument( "Wrong json type for method count." );
}

size_t CJsonValue::size() const
{
  throw invalid_argument( "Wrong json type for method size." );
}

CJsonValue &CJsonValue::operator[]( const string & )
{
  throw invalid_argument( "Wrong json type for operator[ const std::string & ]." );
}

const CJsonValue &CJsonValue::operator[]( const string & ) const
{
  throw invalid_argument( "Wrong json type for operator[ const std::string & ]." );
}

CJsonValue &CJsonValue::operator[]( size_t )
{
  throw invalid_argument( "Wrong json type for operator[ size_t ]." );
}

const CJsonValue &CJsonValue::operator[]( size_t ) const
{
  throw invalid_argument( "Wrong json type for operator[ size_t ]." );
}

CJsonValue::operator bool() const
{
  throw invalid_argument( "Invalid json type for bool cast." );
}


CJsonValue *CJsonValue::parseFromString( const string &data, size_t &position )
{
  parseWhitespace( data, position );
  if( data.size() == position )
    throw invalid_argument( "Expected JSON value but got nothing." );
  CJsonValue *parsedValue;
  if( data[ position ] == '{' )
    parsedValue = CJsonObject::parseFromString( data, position );

  else if( data[ position ] == '[' )
    parsedValue = CJsonArray::parseFromString( data, position );

  else if( data[ position ] == '"' )
    parsedValue = CJsonString::parseFromString( data, position );

  else if( data[ position ] == '-' || ( '0' <= data[ position ] && data[ position ] <= '9' ) )
    parsedValue = CJsonNumber::parseFromString( data, position );

  else if( data[ position ] == 't' || data[ position ] == 'f' )
    parsedValue = CJsonBool::parseFromString( data, position );

  else if( data[ position ] == 'n' )
    parsedValue = CJsonNull::parseFromString( data, position );
  else
    throw invalid_argument( "Expected json value at position " + to_string( position ) );

  parseWhitespace( data, position );
  return parsedValue;
}

void CJsonValue::parseWhitespace( const string &data, size_t &position )
{
  while( position < data.size() && isspace( data[ position ] ) )
    ++position;
}

CJsonValue *CJsonObject::parseFromString( const string &data, size_t &position )
{
  CJsonObject *parsedObject = new CJsonObject;
  if( data[ position ] != '{' )
  {
    delete parsedObject;
    throw invalid_argument( "Expected '{' at position " + to_string( position ) );
  }
  ++position; // '{'

  parseWhitespace( data, position );
  if( data.size() == position )
  {
    delete parsedObject;
    throw invalid_argument( "Expected '}' or string at end of file." );
  }
  if( data[ position ] == '}' )
  {
    ++position;
    return parsedObject;
  }

  while( true )
  {
    parseWhitespace( data, position );
    if( data.size() == position )
    {
      delete parsedObject;
      throw invalid_argument( "Expected string at the end of file." );
    }

    CJsonValue *parsedString;
    string key;
    try
    {
      parsedString = CJsonString::parseFromString( data, position );
      key = move( parsedString->toString() );
      delete parsedString;
    }
    catch( ... )
    {
      delete parsedObject;
      throw;
    }

    parseWhitespace( data, position );
    if( position == data.size() || data[ position ] != ':' )
    {
      delete parsedObject;
      throw invalid_argument( "Expected ':' at position " + to_string( position ) );
    }
    ++position; // ':'

    CJsonValue *parsedValue;
    try
    {
      parsedValue = CJsonValue::parseFromString( data, position );
      parsedObject->m_object.emplace( key, parsedValue );
    }
    catch( ... )
    {
      delete parsedObject;
      throw;
    }

    if( data.size() == position )
    {
      delete parsedObject;
      throw invalid_argument( "Expected ',' or '}' at the end of file." );
    }

    if( data[ position ] == '}' )
      break;
    if( data[ position ] != ',' )
    {
      delete parsedObject;
      throw invalid_argument( "Unexpected character " + to_string( data[ position ] ) +
                              " at position: " + to_string( position ) );
    }
    ++position;
  }
  ++position;
  return parsedObject;
}

size_t CJsonObject::size() const
{
  return m_object.size();
}

size_t CJsonObject::count( const string &key ) const
{
  return m_object.count( key );
}

CJsonValue &CJsonObject::operator[]( const string &key )
{
  return *m_object.at( key );
}

const CJsonValue &CJsonObject::operator[]( const string &key ) const
{
  return *m_object.at( key );
}

CJsonObject::operator bool() const
{
  return !m_object.empty();
}


CJsonValue *CJsonArray::parseFromString( const string &data, size_t &position )
{
  CJsonArray *parsedArray = new CJsonArray;
  if( data[ position ] != '[' )
  {
    delete parsedArray;
    throw invalid_argument( "Expected '[' at position " + to_string( position ) );
  }
  ++position; // '['

  parseWhitespace( data, position );
  if( data.size() == position )
  {
    delete parsedArray;
    throw invalid_argument( "Expected ']' or value at end of file." );
  }
  if( data[ position ] == ']' )
  {
    ++position;
    return parsedArray;
  }

  while( true )
  {
    try
    {
      CJsonValue *parsedValue = CJsonValue::parseFromString( data, position );
      parsedArray->m_vector.emplace_back( parsedValue );
    }
    catch( ... )
    {
      delete parsedArray;
      throw;
    }
    if( data.size() == position )
    {
      delete parsedArray;
      throw invalid_argument( "Expected ',' or ']' at the end of file." );
    }
    if( data[ position ] == ']' )
      break;
    if( data[ position ] != ',' )
    {
      delete parsedArray;
      throw invalid_argument( "Unexpected character " + to_string( data[ position ] ) +
                              " at position: " + to_string( position ) );
    }
    ++position;
  }
  ++position;
  return parsedArray;
}

size_t CJsonArray::size() const
{
  return m_vector.size();
}

CJsonValue &CJsonArray::operator[]( size_t idx )
{
  if( m_vector.size() <= idx )
    throw invalid_argument( "Idx out of range." );
  return *m_vector[ idx ];
}

const CJsonValue &CJsonArray::operator[]( size_t idx ) const
{
  if( m_vector.size() <= idx )
    throw invalid_argument( "Idx out of range." );
  return *m_vector[ idx ];
}

CJsonArray::operator bool() const
{
  return !m_vector.empty();
}

CJsonValue *CJsonString::parseFromString( const string &data, size_t &position )
{
  if( data[ position ] != '"' )
    throw invalid_argument( "Expected '\"' at position " +
                            to_string( position ) );
  ++position; // '"'

  string value;
  while( data.size() > position && data[ position ] != '"' )
  {
    if( iscntrl( data[ position ] ) )
      throw invalid_argument( "Control character inside string at position: " + to_string( position ) );
    if( data[ position ] != '\\' )
    {
      value += data[ position ];
      ++position;
      continue;
    }

    ++position; // '\\'
    if( data.size() == position )
      throw invalid_argument( "Expected character after \\." );
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
      throw invalid_argument( "You are welcome to implement UTF-8." );
    default:
      throw invalid_argument( "Unexpected escape character after \\." );
    }
    ++position;
  }

  if( data.size() == position || data[ position ] != '"' )
    throw invalid_argument( "Expected '\"' at position " + to_string( position ) );
  ++position; // '"'

  return new CJsonString( value );
}

const std::string &CJsonString::toString() const
{
  return m_string;
}

CJsonString::operator bool() const
{
  return !m_string.empty();
}


CJsonValue *CJsonNumber::parseFromString( const string &data, size_t &position )
{
  string number;
  if( data[ position ] == '-' )
  {
    number += '-';
    position++;
  }
  if( data.size() == position )
    throw invalid_argument( "Expected number at position " + to_string( position ) );
  if( data[ position ] == '0' )
    number += data[ position++ ];
  else if( '1' <= data[ position ] && data[ position ] <= '9' )
    while( data.size() != position && '0' <= data[ position ] && data[ position ] <= '9' )
      number += data[ position++ ];
  else
    throw invalid_argument( "Expected number at position " + to_string( position ) );

  if( data.size() == position )
    return new CJsonNumber( number );

  if( data[ position ] == '.' )
  {
    number += data[ position++ ];
    if( data.size() == position || data[ position ] < '0' || data[ position ] > '9' )
      throw invalid_argument( "Expected digit after . at position " + to_string( position ) );
    while( data.size() != position && data[ position ] >= '0' && data[ position ] <= '9' )
      number += data[ position++ ];
  }
  if( data.size() == position )
    return new CJsonNumber( number );

  if( data[ position ] == 'e' || data[ position ] == 'E' )
  {
    number += data[ position++ ];
    if( data.size() == position )
      throw invalid_argument( "Expected digit after E/e at position " + to_string( position ) );
    if( data[ position ] == '+' || data[ position ] == '-' )
      number += data[ position++ ];
    if( data.size() == position || data[ position ] < '0' || data[ position ] > '9' )
      throw invalid_argument( "Expected digit after E/e at position " + to_string( position ) );
    while( data.size() != position && '0' <= data[ position ] && data[ position ] <= '9' )
      number += data[ position++ ];
  }
  return new CJsonNumber( number );
}

int CJsonNumber::toInt() const
{
  return stoi( m_number );
}

double CJsonNumber::toDouble() const
{
  return stod( m_number );
}

CJsonNumber::operator bool() const
{
  return stod( m_number ) != 0;
}


CJsonValue *CJsonBool::parseFromString( const string &data, size_t &position )
{
  if( data.size() >= position + 4 &&
      data[ position ] == 't' &&
      data[ position + 1 ] == 'r' &&
      data[ position + 2 ] == 'u' &&
      data[ position + 3 ] == 'e' )
  {
    position += 4;
    return new CJsonBool( true );
  }
  else if( data.size() >= position + 5 &&
           data[ position ] == 'f' &&
           data[ position + 1 ] == 'a' &&
           data[ position + 2 ] == 'l' &&
           data[ position + 3 ] == 's' &&
           data[ position + 4 ] == 'e' )
  {
    position += 5;
    return new CJsonBool( false );
  }
  else
    throw invalid_argument( "Unexpected character in boolean value at position " + to_string( position ) );
}

CJsonValue *CJsonNull::parseFromString( const string &data, size_t &position )
{
  if( data.size() >= position + 4 &&
      data[ position ] == 'n' &&
      data[ position + 1 ] == 'u' &&
      data[ position + 2 ] == 'u' &&
      data[ position + 3 ] == 'l' )
  {
    position += 4;
    return new CJsonNull();
  }
  else
    throw invalid_argument( "Unexpected character in null value at position " + to_string( position ) );
}

CJsonDocument::CJsonDocument( const string &fileName )
{
  ifstream file( fileName );
  if( !file.good() )
    throw invalid_argument( "File " + fileName + " could not be opened.\n" );
  size_t position = 0;
  string rawText{ istreambuf_iterator<char>( file ),
                  istreambuf_iterator<char>() };

  for( auto c: rawText )
  {
    if( c & 0x80 )
      throw invalid_argument( "UTF-8 characters not supported.\n" );
  }

  m_top = CJsonValue::parseFromString( rawText, position );
  m_type = m_top->m_type;
  file.seekg( 0, ios::end );
  if( position == (size_t)file.tellg() )
    return;
  delete m_top;
  throw invalid_argument( "Expected end of file.\n" );
}

CJsonDocument::~CJsonDocument()
{
  delete m_top;
}