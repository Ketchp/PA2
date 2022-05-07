#pragma once
#include "vector.hpp"

struct physicsAttributes
{
  explicit physicsAttributes( double mass, double angularMass = 0 );
  math::vector velocity;
  double angularVelocity = 0;
  double mass;
  double angularMass;
  math::vector forceAccumulator;
  double momentAccumulator = 0;
};

