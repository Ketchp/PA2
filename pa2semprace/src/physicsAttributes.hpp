#pragma once
#include "linearAlgebra.hpp"
#include <vector>

struct TPhysicsAttributes
{
  static TPhysicsAttributes lineAttributes( double density, TVector<2> direction );
  static TPhysicsAttributes circleAttributes( double density, double radius );
  static TPhysicsAttributes complexObjectAttributes( double density,
                                                     std::vector<TVector<2>> points,
                                                     TVector<2> centreOfMass );
  TVector<2> velocity;
  double angularVelocity = 0;
  double mass;
  double invMass;
  double angularMass;
  double invAngularMass;
  TVector<2> forceAccumulator;
  double momentAccumulator = 0;
  double elasticity = 0.9;
  double frictionCoefficient = 0.1;
private:
  TPhysicsAttributes( double mass, double angularMass );
};
