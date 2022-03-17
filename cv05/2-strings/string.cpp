#include "string.hpp"

#include <cstring>

namespace my_std
{
// ==============================================
// ==================Constructors================
string::string( const char *c_string )
{
  while( *c_string )
    string_space.push_back( *c_string );
};
/*
string::string( const string &other )
  : string_space( other.string_space )
{};

string::string( string &&other )
  : string_space( string_space )
{};
*/

// vytvoreni prazdneho retezce
// vytvoreni kopie retezce predaneho jako const char *
// ==============================================
// ==================Capacity====================
size_t string::size( void ) const
{
  return string_space.size();
};

bool string::empty( void ) const
{
  return string_space.empty();
};

// ==============================================
// ==============Element access==================
const char &string::operator[]( size_t idx ) const
{
  return string_space[ idx ];
};

char &string::operator[]( size_t idx )
{
  return string_space[ idx ];
};

const char *string::data( void ) const
{
  return string_space.data();
};

char *string::data( void )
{
  return string_space.data();
};

const char *string::c_str( void ) const
{
  char *c_string = new char[ size() + 1 ];
  memcpy( c_string, string_space.data(), string_space.size() * sizeof( *c_string ) );
  c_string[ size() ] = '\0';
  return c_string;
};

// ==============================================
// ===============Compare========================
bool operator < ( const string &lhs, const string &rhs )
{
  auto itl = lhs.string_space.cbegin();
  auto itr = rhs.string_space.cbegin();
  while( itl != lhs.string_space.cend() &&
         itr != rhs.string_space.cend() )
  {
    if( *itl < *itr ) return true;
    if( *itl > *itr ) return false;
  }
  return itl != lhs.string_space.cend();
};

bool operator <=( const string &lhs, const string &rhs )
{
  return !( lhs > rhs );
};

bool operator ==( const string &lhs, const string &rhs )
{
  return !( lhs != rhs );
};

bool operator !=( const string &lhs, const string &rhs )
{
  return lhs > rhs || lhs < rhs;
};

bool operator >=( const string &lhs, const string &rhs )
{
  return !( rhs < lhs );
};

bool operator > ( const string &lhs, const string &rhs )
{
  return rhs < lhs;
};

// ==============================================
// ================operations====================
void string::push_back( char c )
{
  string_space.push_back( c );
};

void string::pop_back( void )
{
  string_space.pop_back();
};

void string::clear( void )
{
  string_space.clear();
};

string & string::operator+=( const string &other )
{
  return *this = *this + other;
};

string string::operator+( const string &other ) const
{
  char *c_string = new char[ ( size() + other.size() ) * sizeof( *c_string ) ];
  memcpy( c_string, data(), size() );
  memcpy( c_string + size(), other.data(), other.size() );
  return string( c_string );
};

std::ostream &operator<<( std::ostream &os, const string &str )
{
  for( const char &c : str.string_space )
    os << c;
};

std::istream &operator>>( std::istream &in, string &str )
{

};

} /* namespace my_std */
