#pragma once
#include <cmath>

namespace math
{
struct vector;

struct matrix
{
  vector *data;
  size_t m_width, m_height;

  matrix( std::initializer_list<vector> );

  static matrix rotationMatrix( double angle );

  vector &operator[]( size_t );

  const vector &operator[]( size_t ) const;

  friend vector operator*( const matrix &, const vector & );
};

}

#include "vector.hpp"

namespace math
{

matrix::matrix( std::initializer_list<vector> inList )
  : data( (vector *)new uint8_t[ inList.size() * sizeof( vector ) ] ),
    m_width( inList.size() ),
    m_height( vector::dimension() )
{
  auto it = inList.begin();
  for( size_t idx = 0; idx < m_width ; ++idx, ++it )
    new (data + idx)vector{ *it };
}

matrix matrix::rotationMatrix( double angle )
{
  return { vector{  std::cos( angle ), std::sin( angle ) },
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

