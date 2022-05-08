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

vector::operator bool() const
{
  return data[ 0 ] != NAN  && data[ 1 ] != NAN;
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

double vector::dot( const vector &other ) const
{
  return data[ 0 ] * other[ 0 ] + data[ 1 ] * other[ 1 ];
}

vector &vector::rejectFrom( const vector &other )
{
  return *this = *this - vector( *this ).projectTo( other );
}

vector &vector::projectTo( const vector &other )
{
  return *this = dot( other ) / other.squareNorm() * other;
}


matrix::matrix( std::initializer_list<vector> inList )
        : data( (vector *)new uint8_t[inList.size() * sizeof( vector )] ),
          m_width( inList.size() )
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

double matrix::determinant() const
{
  return data[ 0 ][ 0 ] * data[ 1 ][ 1 ] - data[ 0 ][ 1 ] * data[ 1 ][ 0 ];
}

bool matrix::invert()
{
  double det = determinant();
  if( equalDoubles( det, 0 ) )
    return false;
  std::swap( data[ 0 ][ 0 ], data[ 1 ][ 1 ] );
  std::swap( data[ 0 ][ 1 ], data[ 1 ][ 0 ] );
  data[ 0 ] /= det;
  data[ 1 ] /= det;
  return true;
}

}