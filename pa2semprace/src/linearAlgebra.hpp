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
