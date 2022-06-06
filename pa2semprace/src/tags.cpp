#include "tags.hpp"
#include <type_traits>

using namespace std;

ETag operator|( ETag lhs, ETag rhs )
{
  return static_cast<ETag>(
          static_cast<underlying_type_t<ETag>>( lhs ) |
          static_cast<underlying_type_t<ETag>>( rhs ) );
}

ETag &operator|=( ETag &lhs, ETag rhs )
{
  return lhs = lhs | rhs;
}

ETag operator&( ETag lhs, ETag rhs )
{
  return static_cast<ETag>(
          static_cast<underlying_type_t<ETag>>( lhs ) &
          static_cast<underlying_type_t<ETag>>( rhs ) );
}

ETag &operator&=( ETag &lhs, ETag rhs )
{
  return lhs = lhs & rhs;
}

