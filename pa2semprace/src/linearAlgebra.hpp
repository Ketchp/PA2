#pragma once
#include <array>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <cstdarg>
#include <numeric>

template <size_t dim, typename dataType>
struct TVector;

template <size_t h, size_t w, typename dataType>
struct TMatrix;


template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator+( TVector<fDim, fDataType> lhs,
                                    const TVector<fDim, fDataType> &rhs );

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator-( TVector<fDim, fDataType> lhs,
                                    const TVector<fDim, fDataType> &rhs );

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator*( fDataType lhs, TVector<fDim, fDataType> rhs );

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator*( TVector<fDim, fDataType> lhs, fDataType rhs );

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator/( TVector<fDim, fDataType> lhs, fDataType rhs );

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator/( TVector<fDim, fDataType> lhs, int rhs );

template <size_t h, size_t w, typename dataType>
TVector<h, dataType> operator*( const TMatrix<h,w,dataType> &lhs, const TVector<w,dataType> &rhs );

template <size_t h, size_t w, typename dataType>
TMatrix<h,w, dataType> operator/( TMatrix<h,w, dataType> mat, dataType num );


bool equalDoubles( double a, double b, double precision = 100 )
{
  return std::abs( a - b ) < DBL_EPSILON * ( std::abs( a ) + std::abs( b ) ) * precision;
}

template <size_t dim, typename dataType = double >
struct TVector
{
  std::array<dataType, dim> data;

  TVector();

  TVector( std::initializer_list<dataType> );

  explicit TVector( std::array<dataType, dim> );

  TVector<dim, dataType> &operator+=( const TVector<dim, dataType> & );

  TVector<dim, dataType> &operator-=( const TVector<dim, dataType> & );

  TVector<dim, dataType> &operator*=( dataType );

  TVector<dim, dataType> &operator/=( dataType );

  TVector<dim, dataType> &normalize();

  dataType &operator[]( size_t );

  const dataType &operator[]( size_t ) const;

  explicit operator bool() const;

  [[nodiscard]] dataType norm() const;

  [[nodiscard]] dataType squareNorm() const;

  [[nodiscard]] dataType distance( const TVector<dim, dataType> & ) const;

  [[nodiscard]] bool isZero() const;

  [[nodiscard]] TVector<dim, dataType> rotated( double ) const;

  [[nodiscard]] dataType dot( const TVector<dim, dataType> & ) const;

  TVector<dim, dataType> &rejectFrom( const TVector<dim, dataType> & );

  TVector<dim, dataType> &projectTo( const TVector<dim, dataType> & );

  TVector<dim, dataType> stretchedTo( dataType ) const;

  static size_t dimension() { return dim; };
};

template <size_t h, size_t w, typename dataType = double>
struct TMatrix
{
  std::array<TVector<h, dataType>, w> data;

  TMatrix( std::initializer_list<TVector<h, dataType>> );

  static TMatrix<2,2,double> rotationMatrix2D( double angle );

  TMatrix<h,w, dataType> &operator/=( dataType num );

  TVector<h, dataType> &operator[]( size_t );

  const TVector<h, dataType> &operator[]( size_t ) const;

  [[nodiscard]] dataType det() const;

  bool invert();

  //math::TVector<dim> solveFor( math::TVector<dim> ) const;
};


template <size_t dim, typename dataType>
TVector<dim, dataType>::TVector()
{
  for( size_t idx = 0; idx < dim; ++idx )
    data[ idx ] = dataType( 0 );
}

template <size_t dim, typename dataType>
TVector<dim, dataType>::TVector( std::initializer_list<dataType> initList )
{
  auto it = initList.begin();
  for( size_t idx = 0; idx < dim; ++idx, ++it )
    data[ idx ] = *it;
}

template <size_t dim, typename dataType>
TVector<dim, dataType>::TVector( std::array<dataType, dim> arr )
  : data( std::move( arr ) )
{}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::operator+=( const TVector<dim, dataType> &rhs )
{
  return *this = *this + rhs;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::operator-=( const TVector<dim, dataType> &rhs )
{
  return *this = *this - rhs;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::operator*=( dataType rhs )
{
  return *this = rhs * *this;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::operator/=( dataType rhs )
{
  return *this = *this / rhs;
}

template <size_t dim, typename dataType>
dataType &TVector<dim, dataType>::operator[]( size_t idx )
{
  return data[ idx ];
}

template <size_t dim, typename dataType>
const dataType &TVector<dim, dataType>::operator[]( size_t idx ) const
{
  return data[ idx ];
}

template <size_t dim, typename dataType>
TVector<dim, dataType>::operator bool() const
{
  return std::all_of( data.begin(), data.end(), []( const dataType &elem ){ return elem != NAN; } );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::normalize()
{
  return *this /= norm();
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::norm() const
{
  return std::sqrt( squareNorm() );
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::squareNorm() const
{
  return std::accumulate( data.begin(), data.end(), dataType( 0 ),
                          []( const dataType &a, const dataType &b )
                          {
                            return a + b * b;
                          } );
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::distance( const TVector<dim, dataType> &other ) const
{
  return ( *this - other ).norm();
}

template <size_t dim, typename dataType>
bool TVector<dim, dataType>::isZero() const
{
  return std::all_of( data.begin(), data.end(), []( const dataType &elem ){ return elem == dataType( 0 ); } );
}

template <>
TVector<2, double> TVector<2, double>::rotated( double angle ) const
{
  return TMatrix<2, 2, double>::rotationMatrix2D( angle ) * *this;
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::dot( const TVector<dim, dataType> &other ) const
{
  auto ita = data.begin(),
  itb = other.data.begin();
  dataType temp( 0 );
  while( ita != data.end() )
    temp += *ita++ * *itb;
  return temp;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::rejectFrom( const TVector<dim, dataType> &other )
{
  return *this = *this - TVector<2, dataType>( *this ).projectTo( other );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::projectTo( const TVector<dim, dataType> &other )
{
  return *this = dot( other ) / other.squareNorm() * other;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::stretchedTo( dataType length ) const
{
  return *this * length / norm();
}


template <size_t h, size_t w, typename dataType>
TMatrix<h, w, dataType>::TMatrix( std::initializer_list<TVector<h,dataType>> initList )
{
  auto it = initList.begin();
  for( size_t idx = 0; idx < w; ++idx, ++it )
    data[ idx ] = *it;
}

template <size_t h, size_t w, typename dataType>
TMatrix<2,2,double> TMatrix<h,w,dataType>::rotationMatrix2D( double angle )
{
  return { TVector<2>{ std::cos( angle ), -std::sin( angle ) },
           TVector<2>{ std::sin( angle ),  std::cos( angle ) } };
}

template <size_t h, size_t w, typename dataType>
TMatrix<h,w, dataType> &TMatrix<h,w, dataType>::operator/=( dataType num )
{
  return *this = *this / num;
}


template <size_t h, size_t w, typename dataType>
TVector<h,dataType> &TMatrix<h,w,dataType>::operator[]( size_t idx )
{
  return data[ idx ];
}

template <size_t h, size_t w, typename dataType>
const TVector<h,dataType> &TMatrix<h,w,dataType>::operator[]( size_t idx ) const
{
  return data[ idx ];
}

template <size_t h, size_t w, typename dataType>
dataType TMatrix<h,w,dataType>::det() const
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
  std::swap( data[ 0 ][ 1 ], data[ 1 ][ 0 ] );
  *this /= deter;
  return true;
}


template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator+( TVector<fDim, fDataType> lhs,
                                    const TVector<fDim, fDataType> &rhs )
{
  for( size_t idx = 0; idx < fDim; ++idx )
    lhs[ idx ] += rhs[ idx ];
  return lhs;
}

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator-( TVector<fDim, fDataType> lhs,
                                    const TVector<fDim, fDataType> &rhs )
{
  for( size_t idx = 0; idx < fDim; ++idx )
    lhs[ idx ] -= rhs[ idx ];
  return lhs;
}

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator*( fDataType lhs, TVector<fDim, fDataType> rhs )
{
  for( size_t idx = 0; idx < fDim; ++idx )
    rhs[ idx ] *= lhs;
  return rhs;
}

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator*( TVector<fDim, fDataType> lhs, fDataType rhs )
{
  return rhs * lhs;
}

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator/( TVector<fDim, fDataType> lhs, fDataType rhs )
{
  for( size_t idx = 0; idx < fDim; ++idx )
    lhs[ idx ] /= rhs;
  return lhs;
}

template <size_t fDim, typename fDataType>
TVector<fDim, fDataType> operator/( TVector<fDim, fDataType> lhs, int rhs )
{
  for( size_t idx = 0; idx < fDim; ++idx )
    lhs[ idx ] /= rhs;
  return lhs;
}


template <size_t h, size_t w, typename dataType>
TVector<h, dataType> operator*( const TMatrix<h,w,dataType> &lhs, const TVector<w,dataType> &rhs )
{
  TVector<h, dataType> result;
  for( size_t i = 0; i < w; ++i )
    result += rhs[ i ] * lhs[ i ];
  return TVector<h,dataType>( result );
}

template <size_t h, size_t w, typename dataType>
TMatrix<h,w, dataType> operator/( TMatrix<h,w, dataType> mat, dataType num )
{
  for( size_t idx = 0; idx < w; ++idx )
    mat.data[ idx ] /= num;
  return mat;
}
