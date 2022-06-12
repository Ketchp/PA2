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

/**
 * @tparam fDim vector dimension
 * @tparam fDataType vector dataType
 * @param lhs
 * @param rhs
 * @return sum of lhs and rhs
 */
template <size_t fDim, typename fDataType>
inline TVector<fDim, fDataType> operator+( TVector<fDim, fDataType> lhs,
                                           const TVector<fDim, fDataType> &rhs );

/**
 * @tparam fDim vector dimension
 * @tparam fDataType vector dataType
 * @param lhs
 * @param rhs
 * @return lhs minus rhs
 */
template <size_t fDim, typename fDataType>
inline TVector<fDim, fDataType> operator-( TVector<fDim, fDataType> lhs,
                                           const TVector<fDim, fDataType> &rhs );

/**
 * @tparam fDim vector dimension
 * @tparam fDataType vector dataType
 * @param lhs
 * @param rhs
 * @return rhs multiplied by lhs
 */
template <size_t fDim, typename fDataType>
inline TVector<fDim, fDataType> operator*( fDataType lhs, TVector<fDim, fDataType> rhs );

/**
 * @tparam fDim vector dimension
 * @tparam fDataType vector dataType
 * @param lhs
 * @param rhs
 * @return lhs multiplied by rhs;
 */
template <size_t fDim, typename fDataType>
inline TVector<fDim, fDataType> operator*( TVector<fDim, fDataType> lhs, fDataType rhs );

/**
 * @tparam fDim vector dimension
 * @tparam fDataType vector dataType
 * @param lhs
 * @param rhs
 * @return lhs divided by rhs
 */
template <size_t fDim, typename fDataType>
inline TVector<fDim, fDataType> operator/( TVector<fDim, fDataType> lhs, fDataType rhs );

/**
 * @tparam fDim vector dimension
 * @tparam fDataType vector dataType
 * @param lhs
 * @param rhs
 * @return lhs divided by rhs
 */
template <size_t fDim, typename fDataType>
inline TVector<fDim, fDataType> operator/( TVector<fDim, fDataType> lhs, int rhs );

/**
 *
 * @tparam h matrix height
 * @tparam w matrix width
 * @tparam dataType matrix and vector dataType
 * @param lhs
 * @param rhs
 * @return linear combination of matrix column-space
 */
template <size_t h, size_t w, typename dataType>
inline TVector<h, dataType> operator*( const TMatrix<h, w, dataType> &lhs, const TVector<w, dataType> &rhs );

/**
 *
 * @tparam h matrix height
 * @tparam w matrix width
 * @tparam dataType matrix and vector dataType
 * @param mat
 * @param num
 * @return matrix divided by num
 */
template <size_t h, size_t w, typename dataType>
inline TMatrix<h, w, dataType> operator/( TMatrix<h, w, dataType> mat, dataType num );

/**
 * Compares double with precision of precision * epsilon.
 * @param a
 * @param b
 * @param precision
 * @return true if a, b are almost equal.
 */
bool equalDoubles( double a, double b, double precision = 100 );

template <typename dataType>
inline TVector<2, dataType> crossProduct( const TVector<2, dataType> &first )
{
  return { -first[ 1 ], first[ 0 ] };
}

template <size_t dim, typename dataType>
inline std::ostream &operator<<( std::ostream &os, const TVector<dim, dataType> &vec )
{
  os << std::string( "( " );
  os << vec[ 0 ];
  for( size_t idx = 1; idx < dim; ++idx )
    os << std::string( ", " ) << vec[ idx ];
  return os << std::string( " )" );
}

/**
 * Class for representation of mathematical vector
 * @tparam dim vector dimension
 * @tparam dataType dataType of base field
 */
template <size_t dim, typename dataType = double>
struct TVector
{
  /**
   * Array of vector values.
   */
  std::array<dataType, dim> data;

  /**
   * Vector with all values initialised to 0;
   */
  TVector();

  /**
   * Vector initialised from list.
   * @param initList
   */
  TVector( std::initializer_list<dataType> initList );

  /**
   * vector initialised from array.
   * @param initArr
   */
  explicit TVector( std::array<dataType, dim> initArr );

  /**
   * Add other vector to this.
   * @param rhs other vector
   * @return *this
   */
  inline TVector<dim, dataType> &operator+=( const TVector<dim, dataType> &rhs );

  /**
   * Subtracts other vector from this.
   * @param rhs other vector
   * @return *this
   */
  inline TVector<dim, dataType> &operator-=( const TVector<dim, dataType> &rhs );

  /**
   * Multiplies vector by rhs.
   * @param rhs
   * @return *this
   */
  inline TVector<dim, dataType> &operator*=( dataType rhs );

  /**
   * Divides vector by rhs.
   * @param rhs
   * @return *this
   */
  inline TVector<dim, dataType> &operator/=( dataType rhs );

  /**
   * @return Negative vector
   */
  inline TVector<dim, dataType> operator-() const;

  /**
   * Normalises vector.
   * @return *this
   */
  inline TVector<dim, dataType> &normalize();

  /**
   * @return normalized vector
   */
  [[nodiscard]] inline TVector<dim, dataType> normalized() const;

  /**
   * @param index
   * @return Value at position index.
   */
  inline dataType &operator[]( size_t index );

  /**
   * @param index
   * @return Value at position index.
   */
  inline const dataType &operator[]( size_t index ) const;

  /**
   * @return true if vector is valid.
   */
  inline explicit operator bool() const;

  /**
   * @return Norm of vector.
   */
  [[nodiscard]] inline dataType norm() const;

  /**
   * @return Squared norm of vector.
   */
  [[nodiscard]] inline dataType squareNorm() const;

  /**
   * @param other
   * @return Euclidean distance from other vector.
   */
  [[nodiscard]] inline dataType distance( const TVector<dim, dataType> &other ) const;

  /**
   * @param other
   * @return Squared Euclidean distance from other vector.
   */
  [[nodiscard]] inline dataType squareDistance( const TVector<dim, dataType> &other ) const;

  /**
   * @param other
   * @return Dot product with other vector.
   */
  [[nodiscard]] inline dataType dot( const TVector<dim, dataType> &other ) const;

  /**
   * Rotates vector by angle.( Counter clockwise )
   * @param angle
   * @return *this
   */
  TVector<dim, dataType> &rotate( double angle );

  /**
   * @param angle
   * @return Vector rotated by angle. ( Counter clockwise )
   */
  [[nodiscard]] TVector<dim, dataType> rotated( double angle ) const;

  /**
   * Makes vector orthogonal to other,
   * whilst preserving distance from space generated by other.
   * @param other
   * @return *this
   */
  inline TVector<dim, dataType> &rejectFrom( const TVector<dim, dataType> &other );

  /**
   * @param other
   * @return Orthogonal vector to other with same distance from span( other ) as *this
   */
  [[nodiscard]] inline TVector<dim, dataType> rejectedFrom( const TVector<dim, dataType> &other ) const;

  /**
   * Projects vector to other
   * @param other
   * @return *this
   */
  inline TVector<dim, dataType> &projectTo( const TVector<dim, dataType> &other );

  /**
   * @param other
   * @return Closest point from span( other ) as by distance induced from standard norm.
   */
  [[nodiscard]] inline TVector<dim, dataType> projectedTo( const TVector<dim, dataType> &other ) const;

  /**
   * Stretches vector to have norm of length.
   * @param length
   * @return *this
   */
  inline TVector<dim, dataType> &stretchTo( dataType length );

  /**
   * @param length
   * @return this vector stretched to length.
   */
  [[nodiscard]] inline TVector<dim, dataType> stretchedTo( dataType length ) const;

  /**
   * @param n
   * @return Canonical vector in axis n.
   */
  inline static TVector<dim, dataType> canonical( size_t n );

  /**
   * @param n
   * @param size
   * @return Canonical vector in axis n stretched to size.
   */
  inline static TVector<dim, dataType> canonical( size_t n, dataType size );

  /**
   * @return Clockwise distance from canonical vector.
   */
  [[nodiscard]] double getAngle() const;
};

/**
 * Class for representation of matrices.
 * @tparam h dimension of columns
 * @tparam w dimension of rows
 * @tparam dataType dataType of base field
 */
template <size_t h, size_t w, typename dataType = double>
struct TMatrix
{
  /**
   * Array of column vectors.
   */
  std::array<TVector<h, dataType>, w> data;

  /**
   * Initialises matrix with column vectors.
   * @param initList
   */
  TMatrix( std::initializer_list<TVector<h, dataType>> initList );

  /**
   * @param angle
   * @return Rotation matrix.
   */
  inline static TMatrix<2, 2, double> rotationMatrix2D( double angle );

  /**
   * Divides matrix by num.
   * @param num
   * @return *this
   */
  inline TMatrix<h, w, dataType> &operator/=( dataType num );

  /**
   * @param index
   * @return Vector at index column.
   */
  inline TVector<h, dataType> &operator[]( size_t index );

  /**
   * @param index
   * @return Vector at index column.
   */
  inline const TVector<h, dataType> &operator[]( size_t index ) const;

  /**
   * @return Matrix determinant.
   */
  [[nodiscard]] inline dataType det() const;

  /**
   * Inverts matrix.
   * @return true on success
   * @return false if matrix not regular
   */
  [[nodiscard]] bool invert();
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
        : data( std::move( arr ) ){}

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
dataType &TVector<dim, dataType>::operator[]( size_t index )
{
  return data[ index ];
}

template <size_t dim, typename dataType>
const dataType &TVector<dim, dataType>::operator[]( size_t index ) const
{
  return data[ index ];
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
TMatrix<h, w, dataType>::TMatrix( std::initializer_list<TVector<h, dataType>> initList )
{
  auto it = initList.begin();
  for( size_t idx = 0; idx < w; ++idx, ++it )
    data[ idx ] = *it;
}

template <size_t h, size_t w, typename dataType>
TMatrix<2, 2, double> TMatrix<h, w, dataType>::rotationMatrix2D( double angle )
{
  return { TVector<2>{ std::cos( angle ), -std::sin( angle ) },
           TVector<2>{ std::sin( angle ), std::cos( angle ) } };
}

template <size_t h, size_t w, typename dataType>
TMatrix<h, w, dataType> &TMatrix<h, w, dataType>::operator/=( dataType num )
{
  return *this = *this / num;
}


template <size_t h, size_t w, typename dataType>
TVector<h, dataType> &TMatrix<h, w, dataType>::operator[]( size_t index )
{
  return data[ index ];
}

template <size_t h, size_t w, typename dataType>
const TVector<h, dataType> &TMatrix<h, w, dataType>::operator[]( size_t index ) const
{
  return data[ index ];
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
TVector<h, dataType> operator*( const TMatrix<h, w, dataType> &lhs, const TVector<w, dataType> &rhs )
{
  TVector<h, dataType> result;
  for( size_t i = 0; i < w; ++i )
    result += rhs[ i ] * lhs[ i ];
  return TVector<h, dataType>( result );
}

template <size_t h, size_t w, typename dataType>
TMatrix<h, w, dataType> operator/( TMatrix<h, w, dataType> mat, dataType num )
{
  for( size_t idx = 0; idx < w; ++idx )
    mat.data[ idx ] /= num;
  return mat;
}
