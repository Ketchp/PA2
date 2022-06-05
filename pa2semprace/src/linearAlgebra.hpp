#pragma once
#include <cmath>
#include <array>
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


bool equalDoubles( double a, double b, double precision = 100 );

template <size_t dim, typename dataType>
TVector<dim, dataType> crossProduct( std::initializer_list<TVector<dim, dataType>> args );

template <typename dataType>
TVector<2, dataType> crossProduct( const TVector<2, dataType> &first )
{
  return crossProduct( { first } );
}

template <typename dataType>
TVector<3, dataType> crossProduct( const TVector<3, dataType> &first,
                                   const TVector<3, dataType> &second )
{
  return crossProduct( { first, second } );
}

template <size_t dim, typename dataType>
std::ostream &operator<<( std::ostream &os, const TVector<dim, dataType> &vec )
{
  os << std::string( "( " );
  os << vec[ 0 ];
  for( size_t idx = 1; idx < dim; ++idx )
    os << std::string( ", " ) << vec[ idx ];
  return os << std::string( " )" );
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

  TVector<dim, dataType> operator-() const;

  TVector<dim, dataType> &normalize();

  TVector<dim, dataType> normalized() const;

  dataType &operator[]( size_t );

  const dataType &operator[]( size_t ) const;

  explicit operator bool() const;

  [[nodiscard]] dataType norm() const;

  [[nodiscard]] dataType squareNorm() const;

  [[nodiscard]] dataType distance( const TVector<dim, dataType> & ) const;

  [[nodiscard]] dataType squareDistance( const TVector<dim, dataType> & ) const;

  [[nodiscard]] bool isZero() const;

  [[nodiscard]] dataType dot( const TVector<dim, dataType> & ) const;

  TVector<dim, dataType> &rotate( double );
  [[nodiscard]] TVector<dim, dataType> rotated( double ) const;

  TVector<dim, dataType> &rejectFrom( const TVector<dim, dataType> & );
  [[nodiscard]] TVector<dim, dataType> rejectedFrom( const TVector<dim, dataType> & ) const;

  TVector<dim, dataType> &projectTo( const TVector<dim, dataType> & );
  [[nodiscard]] TVector<dim, dataType> projectedTo( const TVector<dim, dataType> & ) const;

  TVector<dim, dataType> &stretchTo( dataType );
  [[nodiscard]] TVector<dim, dataType> stretchedTo( dataType ) const;

  template <size_t inDim>
  static TVector<dim, dataType> changeDim( const TVector<inDim, dataType> &input );

  static TVector<dim, dataType> canonical( size_t n );

  static TVector<dim, dataType> canonical( size_t n, dataType size );

  static size_t dimension() { return dim; };

  double getAngle() const;

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
TVector<dim, dataType> TVector<dim, dataType>::operator-() const
{
  return *this * -1.;
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
  auto pred = []( const dataType &elem )
  {
    return std::isnan( elem );
  };
  return !std::any_of( data.begin(), data.end(), pred );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::normalize()
{
  return *this = normalized();
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::normalized() const
{
  return *this / norm();
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::norm() const
{
  return std::sqrt( squareNorm() );
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::squareNorm() const
{
  dataType val( 0 );
  for( const dataType &elem: data )
    val += elem * elem;
  return val;
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::distance( const TVector<dim, dataType> &other ) const
{
  return ( *this - other ).norm();
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::squareDistance( const TVector<dim, dataType> &other ) const
{
  return ( *this - other ).squareNorm();
}

template <size_t dim, typename dataType>
bool TVector<dim, dataType>::isZero() const
{
  return std::all_of( data.begin(), data.end(), []( const dataType &elem ){ return elem == dataType( 0 ); } );
}

template <size_t dim, typename dataType>
dataType TVector<dim, dataType>::dot( const TVector<dim, dataType> &other ) const
{
  dataType temp( 0 );
  for( auto ita = data.begin(), itb = other.data.begin();
       ita != data.end();
       ++ita, ++itb )
    temp += *ita * *itb;
  return temp;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::rejectFrom( const TVector<dim, dataType> &other )
{
  return *this = this->rejectedFrom( other );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::rejectedFrom( const TVector<dim, dataType> &other ) const
{
  return *this - this->projectedTo( other );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::projectTo( const TVector<dim, dataType> &other )
{
  return *this = this->projectedTo( other );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::projectedTo( const TVector<dim, dataType> &other ) const
{
  return dot( other ) / other.squareNorm() * other;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> &TVector<dim, dataType>::stretchTo( dataType length )
{
  return *this = this->stretchedTo( length );
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::stretchedTo( dataType length ) const
{
  return *this * length / norm();
}

template <size_t dim, typename dataType>
template <size_t inDim>
TVector<dim, dataType> TVector<dim, dataType>::changeDim( const TVector<inDim, dataType> &input )
{
  TVector<dim, dataType> out;
  for( size_t idx = 0; idx < std::min( dim, inDim ); ++idx )
    out[ idx ] = input[ idx ];
  return out;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::canonical( size_t n )
{
  auto res = TVector<dim, dataType>();
  res[ n ] = 1;
  return res;
}

template <size_t dim, typename dataType>
TVector<dim, dataType> TVector<dim, dataType>::canonical( size_t n, dataType size )
{
  auto res = TVector<dim, dataType>();
  res[ n ] = size;
  return res;
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
