#include "linearAlgebra.hpp"


using namespace std;

bool equalDoubles( double a, double b, double precision )
{
  return abs( a - b ) < DBL_EPSILON * ( 1 + abs( a ) + abs( b ) ) * precision;
}

/**
 * Specialisation for 2d rotation.
 * @param angle
 * @return Vector rotated by angle. ( Counter clockwise )
 */
template <>
TVector<2, double> TVector<2, double>::rotated( double angle ) const
{
  return TMatrix<2, 2, double>::rotationMatrix2D( angle ) * *this;
}

/**
 * Specialisation for 2d rotation.
 * Rotates vector by angle.( Counter clockwise )
 * @param angle
 * @return *this
 */
template <>
TVector<2, double> &TVector<2, double>::rotate( double angle )
{
  return *this = this->rotated( angle );
}

/**
 * Specialisation for 2d.
 * @return Clockwise distance from canonical vector.
 */
template <>
double TVector<2, double>::getAngle() const
{
  return -atan2( data[ 1 ], data[ 0 ] );
}

/**
 * Specialisation for determinant of 2 by 2 matrices
 * @return Matrix determinant.
 */
template <>
double TMatrix<2, 2, double>::det() const
{
  return data[ 0 ][ 0 ] * data[ 1 ][ 1 ] - data[ 0 ][ 1 ] * data[ 1 ][ 0 ];
}

/**
 * Specialisation for invert of 2 by 2 matrix
 * @return false if matrix not regular
 */
template <>
bool TMatrix<2, 2, double>::invert()
{
  double deter = det();
  if( equalDoubles( deter, 0 ) )
    return false;
  std::swap( data[ 0 ][ 0 ], data[ 1 ][ 1 ] );
  data[ 0 ][ 1 ] *= -1;
  data[ 1 ][ 0 ] *= -1;
  *this /= deter;
  return true;
}
