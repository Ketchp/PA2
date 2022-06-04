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
  TManifold( CObject *first, CObject *second );

  TManifold( CObject *first, CObject *second,
             TVector<2> overlapVector,
             TVector<2> contactPoint );

  TManifold( CObject *first, CObject *second,
             const TContactPoint & );

  explicit operator bool() const
  {
    return first && second;
  };

  CPhysicsObject *first;
  CPhysicsObject *second;
  std::vector<TContactPoint> contacts;
};
