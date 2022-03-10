#include "fraction.hpp"
#include <numeric>

CFraction::CFraction( int nominator, int denominator )
    : nominator( nominator ), denominator( denominator )
{
  if( !denominator ) denominator = 1;
  normalize();
}

CFraction operator+( const CFraction &lhs, const CFraction &rhs )
{
  return CFraction( lhs.nominator * rhs.denominator + lhs.denominator * rhs.nominator,
                          lhs.denominator * rhs.denominator );
}

CFraction operator-( const CFraction &lhs, const CFraction &rhs )
{
  return lhs + (-rhs);
}

CFraction operator*( const CFraction &lhs, const CFraction &rhs )
{
  return CFraction( lhs.nominator * rhs.nominator, lhs.denominator * rhs.denominator );
}

CFraction operator/( const CFraction &lhs, const CFraction &rhs )
{
  return CFraction( lhs.nominator * rhs.denominator, rhs.nominator * lhs.denominator );
}

CFraction &CFraction::operator+=( const CFraction &other )
{
  return *this = *this + other;
}

CFraction &CFraction::operator-=( const CFraction &other )
{
  return *this = *this - other;
}

CFraction &CFraction::operator*=( const CFraction &other )
{
  return *this = *this * other;
}

CFraction &CFraction::operator/=( const CFraction &other )
{
  return *this = *this / other;
}

CFraction CFraction::operator++( int n )
{
  CFraction copy( *this );
  *this += n ? n : 1;
  return copy;
}

CFraction CFraction::operator--( int n )
{
  CFraction copy( *this );
  *this -= n ? n : 1 ;
  return copy;
}

CFraction &CFraction::operator++( void )
{
  *this += 1;
  return *this;
}

CFraction &CFraction::operator--( void )
{
  *this -= 1;
  return *this;
}

CFraction CFraction::operator-( void ) const
{
  return CFraction( -nominator, denominator );
}

bool operator< ( const CFraction &lhs, const CFraction &rhs )
{
  return lhs.nominator * rhs.denominator < rhs.nominator * lhs.denominator;
}

bool operator<=( const CFraction &lhs, const CFraction &rhs )
{
  return !( lhs > rhs );
}

bool operator==( const CFraction &lhs, const CFraction &rhs )
{
  return !( lhs > rhs ) && !( lhs < rhs );
}

bool operator!=( const CFraction &lhs, const CFraction &rhs )
{
  return !( lhs == rhs );
}

bool operator>=( const CFraction &lhs, const CFraction &rhs )
{
  return !( lhs < rhs );
}

bool operator> ( const CFraction &lhs, const CFraction &rhs )
{
  return rhs < lhs;
}

std::ostream &operator<<( std::ostream & out, const CFraction &frac )
{
  return out << frac.nominator << '/' << frac.denominator;
}

std::istream &operator>>( std::istream & in, CFraction &frac )
{
  int nom, denom;
  char separ;

  in >> nom;

  if( !in ) return in;

  if( in.peek() != '/' )
  {
    frac = CFraction(nom);
    return in;
  }

  in.get( separ );

  in >> denom;
  if( !denom ) in.setstate( std::ios::failbit | in.badbit );
  if( in ) frac = CFraction( nom, denom );
  return in;
}

void CFraction::normalize( void )
{
  int a = std::abs( denominator ), b = std::abs( nominator );
  while( b != 0 )
  {
    a %= b;
    a ^= b;
    b ^= a;
    a ^= b;
  }
  nominator /= a * sign( denominator );
  denominator /= a * sign( denominator );
}

template <typename T>
int sign( T val )
{
    return ( T( 0 ) < val ) - ( val < T( 0 ) );
}
