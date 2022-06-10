#pragma once
#include <cstdint>

enum ETag : uint8_t
{
  NONE = 0,

  // you can draw over transparent objects, they return HUGE_VAL for ray cast test
  TRANSPARENT = 1,

  // only two solid objects can collide
  // manifold is created also for non-solid pairs
  // to do checks but impulses are never applied
  NON_SOLID = 2,

  // value intended for use with checks
  TARGET = 4
};

ETag operator|( ETag lhs, ETag rhs );
ETag &operator|=( ETag &lhs, ETag rhs );
ETag operator&( ETag lhs, ETag rhs );
ETag &operator&=( ETag &lhs, ETag rhs );

