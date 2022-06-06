#pragma once
#include <cstdint>

enum ETag : uint8_t
{
  NONE = 0,
  WIN_ZONE = 1,
  NO_DRAW_ZONE = 2,
  ZONE = 3,
  DRAWN = 4
};

ETag operator|( ETag lhs, ETag rhs );
ETag &operator|=( ETag &lhs, ETag rhs );
ETag operator&( ETag lhs, ETag rhs );
ETag &operator&=( ETag &lhs, ETag rhs );

