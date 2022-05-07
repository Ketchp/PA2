#pragma once
#include <array>
#include <cmath>
#include <algorithm>
#include <cstdarg>

namespace math
{

struct vector
{
  double data[2] = { 0, 0 };

  vector() = default;

  vector( double first, double second );
  vector( int first, int second )
    : vector( (double)first, (double)second )
  {};

  friend vector operator+( const vector &, const vector & );

  friend vector operator-( const vector &, const vector & );

  friend vector operator*( double, const vector & );

  friend vector operator/( const vector &, double );

  vector &operator+=( const vector & );

  vector &operator-=( const vector & );

  vector &operator*=( double rhs );

  vector &operator/=( double );

  vector &normalize();
  
  double &operator[]( size_t idx );

  double operator[]( size_t idx ) const;

  [[nodiscard]] double norm() const;

  [[nodiscard]] double squareNorm() const;

  [[nodiscard]]double distance( const vector & ) const;

  [[nodiscard]]bool isZero() const;

  [[nodiscard]] vector rotated( double angle ) const;
  
  static size_t dimension() { return 2; };
};

}

#include "matrix.hpp"

namespace math
{

vector::vector( double first, double second )
{
  data[0] = first;
  data[1] = second;
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
  return data[ 0 ]*data[ 0 ] + data[ 1 ]*data[ 1 ];
}

double vector::distance( const vector &other ) const
{
  return (*this - other).norm();
}

vector vector::rotated( double angle ) const
{
  return matrix::rotationMatrix( angle ) * *this;
}

bool vector::isZero() const
{
  return data[ 0 ] == 0 && data[ 1 ] == 0;
}


}
