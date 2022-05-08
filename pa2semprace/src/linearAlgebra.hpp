#pragma once
#include <array>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <cstdarg>

namespace math
{

bool equalDoubles( double a, double b, size_t precision = 1000 )
{
  return b - a  < DBL_EPSILON && a - b < DBL_EPSILON;
}

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

  explicit operator bool() const;

  [[nodiscard]] double norm() const;

  [[nodiscard]] double squareNorm() const;

  [[nodiscard]]double distance( const vector & ) const;

  [[nodiscard]]bool isZero() const;

  [[nodiscard]] vector rotated( double angle ) const;

  [[nodiscard]] double dot( const vector & ) const;

  vector &rejectFrom( const vector & );

  vector &projectTo( const vector & );

  static size_t dimension() { return 2; };
};

struct matrix
{
  vector *data;
  size_t m_width;

  matrix( std::initializer_list<vector> );

  static matrix rotationMatrix( double angle );

  vector &operator[]( size_t );

  const vector &operator[]( size_t ) const;

  [[nodiscard]] double determinant() const;

  bool invert();

  //math::vector solveFor( math::vector ) const;

  friend vector operator*( const matrix &, const vector & );
};


}
