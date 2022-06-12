#include "linearAlgebra.hpp"


using namespace std;

bool equalDoubles( double a, double b, double precision )
{
  return abs( a - b ) < DBL_EPSILON * ( 1 + abs( a ) + abs( b ) ) * precision;
}

template <>
TVector<2, double> TVector<2, double>::rotated( double angle ) const
{
  return TMatrix<2, 2, double>::rotationMatrix2D( angle ) * *this;
}

template <>
TVector<2, double> &TVector<2, double>::rotate( double angle )
{
  return *this = this->rotated( angle );
}

template <>
double TVector<2, double>::getAngle() const
{
  return -atan2( data[ 1 ], data[ 0 ] );
}

template <>
double TMatrix<2, 2, double>::det() const
{
  return data[ 0 ][ 0 ] * data[ 1 ][ 1 ] - data[ 0 ][ 1 ] * data[ 1 ][ 0 ];
}

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
