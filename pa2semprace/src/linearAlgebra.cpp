#include "linearAlgebra.hpp"

using namespace std;

bool equalDoubles( double a, double b, double precision )
{
  return abs( a - b ) < DBL_EPSILON * ( 1 + abs( a ) + abs( b ) ) * precision;
}

template <>
TVector<3> crossProduct( std::initializer_list<TVector<3>> args )
{
  auto it = args.begin();
  TVector<3> a = *it;
  TVector<3> b = *++it;
  return { a[1] * b[2] - a[2] * b[1],
           a[2] * b[0] - a[0] * b[2],
           a[0] * b[1] - a[1] * b[0] };
}

template <>
TVector<2> crossProduct( std::initializer_list<TVector<2>> args )
{
  TVector<2> a = *args.begin();
  return { -a[1], a[0] };
}

template <>
TVector<2, double> TVector<2, double>::rotated( double angle ) const
{
  return TMatrix<2, 2, double>::rotationMatrix2D( angle ) * *this;
}

template <>
double TMatrix<2,2,double>::det() const
{
  return data[ 0 ][ 0 ] * data[ 1 ][ 1 ] - data[ 0 ][ 1 ] * data[ 1 ][ 0 ];
}

template <>
bool TMatrix<2,2,double>::invert()
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
