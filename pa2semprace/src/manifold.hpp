#pragma once

#include <functional>
#include <vector>
#include "linearAlgebra.hpp"


class CObject;

class CPhysicsObject;

struct TContactPoint
{
  TVector<2> overlapVector; // distance needed to push second object
  TVector<2> contactPoint;
};

struct TManifold
{
  TManifold( CPhysicsObject *first, CPhysicsObject *second );

  TManifold( CPhysicsObject *first, CPhysicsObject *second,
             TVector<2> overlapVector,
             TVector<2> contactPoint );

  TManifold( CPhysicsObject *first, CPhysicsObject *second,
             const TContactPoint & );

  TManifold( CPhysicsObject *first, CPhysicsObject *second,
             std::vector<TContactPoint> contacts );

  explicit operator bool() const
  {
    return first && second;
  };

  CPhysicsObject *first;
  CPhysicsObject *second;
  std::vector<TContactPoint> contacts;
};
