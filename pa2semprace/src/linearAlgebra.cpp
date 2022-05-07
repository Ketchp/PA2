#include "linearAlgebra.hpp"

namespace math
{

vector::vector( double first, double second )
{
  data[ 0 ] = first;
  data[ 1 ] = second;
}

vector operator+( const vector &lhs, const vector &rhs )
{
  return { lhs[ 0 ] + rhs[ 0 ], lhs[ 1 ] + rhs[ 1 ] };
}

vector operator-( const vector &lhs, const vector &rhs )
{
  return { lhs[ 0 ] - rhs[ 0 ], lhs[ 1 ] - rhs[ 1 ] };
}

vector operator*( double lhs, const vector &rhs )
{
  return { lhs * rhs[ 0 ], lhs * rhs[ 1 ] };
}

vector operator/( const vector &lhs, double rhs )
{
  return { lhs[ 0 ] / rhs, lhs[ 1 ] / rhs };
}

vector &vector::operator+=( const vector &rhs )
{
  return *this = *this + rhs;
}

vector &vector::operator-=( const vector &rhs )
{
  return *this = *this - rhs;
}

vector &vector::operator*=( double rhs )
{
  return *this = rhs * *this;
}

vector &vector::operator/=( double rhs )
{
  return *this = *this / rhs;
}

double &vector::operator[]( size_t idx )
{
  return data[ idx ];
}

double vector::operator[]( size_t idx ) const
{
  return data[ idx ];
}


vector &vector::normalize()
{
  return *this /= norm();
}

double vector::norm() const
{
  return std::sqrt( squareNorm() );
}

double vector::squareNorm() const
{
  return data[ 0 ] * data[ 0 ] + data[ 1 ] * data[ 1 ];
}

double vector::distance( const vector &other ) const
{
  return ( *this - other ).norm();
}

vector vector::rotated( double angle ) const
{
  return matrix::rotationMatrix( angle ) * *this;
}

bool vector::isZero() const
{
  return data[ 0 ] == 0 && data[ 1 ] == 0;
}


matrix::matrix( std::initializer_list<vector> inList )
        : data( (vector *)new uint8_t[inList.size() * sizeof( vector )] ),
          m_width( inList.size() ),
          m_height( vector::dimension() )
{
  auto it = inList.begin();
  for( size_t idx = 0; idx < m_width; ++idx, ++it )
    new( data + idx )vector{ *it };
}

matrix matrix::rotationMatrix( double angle )
{
  return { vector{ std::cos( angle ), std::sin( angle ) },
           vector{ -std::sin( angle ), std::cos( angle ) } };
}

vector &matrix::operator[]( size_t idx )
{
  return data[ idx ];
}

const vector &matrix::operator[]( size_t idx ) const
{
  return data[ idx ];
}

vector operator*( const matrix &lhs, const vector &rhs )
{
  vector result( rhs[ 0 ] * lhs[ 0 ] );
  for( size_t i = 1; i < lhs.m_width; ++i )
    result += rhs[ i ] * lhs[ i ];
  return result;
}


}