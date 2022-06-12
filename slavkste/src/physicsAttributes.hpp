#pragma once

#include "linearAlgebra.hpp"
#include <vector>


struct TPhysicsAttributes
{
  TPhysicsAttributes( double mass, double angularMass );

  static TPhysicsAttributes rectangleAttributes( double density, const TVector<2> &size );

  static TPhysicsAttributes circleAttributes( double density, double radius );

  static TPhysicsAttributes complexObjectAttributes( double width,
                                                     double density,
                                                     std::vector<TVector<2>> points );

  TVector<2> velocity;
  double angularVelocity = 0;
  double mass;
  double invMass;
  double angularMass;
  double invAngularMass;
  TVector<2> forceAccumulator;
  double momentAccumulator = 0;
  double elasticity = 0.8;
  double frictionCoefficient = 0.5;
  double integrity = 1;
};
