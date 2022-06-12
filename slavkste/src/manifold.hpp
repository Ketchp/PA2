#pragma once

#include <functional>
#include <vector>
#include "linearAlgebra.hpp"


class CPhysicsObject;

/**
 * Information about single contact between two objects,
 * size and direction of overlap, place of overlap
 */
struct TContactPoint
{
  /**
   * Distance needed to push second object,
   * from whom the collision was calculated.
   */
  TVector<2> overlapVector;

  /**
   * Centre point of contact.
   */
  TVector<2> contactPoint;
};


/**
 * Structure for storing all necessary information for collision resolution.
 */
struct TManifold
{
  /**
   * Construct manifold with no contacts points.
   * @param first
   * @param second
   */
  TManifold( CPhysicsObject *first, CPhysicsObject *second );

  /**
   * Construct manifold with single contact point.
   * @param first
   * @param second
   * @param overlapVector
   * @param contactPoint
   */
  TManifold( CPhysicsObject *first, CPhysicsObject *second,
             TVector<2> overlapVector,
             TVector<2> contactPoint );

  /**
   * Construct manifold with single contact point.
   * @param first
   * @param second
   * @param contactPoint
   */
  TManifold( CPhysicsObject *first, CPhysicsObject *second,
             const TContactPoint &contactPoint );

  /**
   * Construct manifold with contacts.
   * @param first
   * @param second
   * @param contacts
   */
  TManifold( CPhysicsObject *first, CPhysicsObject *second,
             std::vector<TContactPoint> contacts );

  /**
   * @return true if manifold is valid
   */
  explicit operator bool() const
  {
    return first && second;
  };

  /**
   * Colliding object.
   */
  CPhysicsObject *first;

  /**
   * Colliding object.
   */
  CPhysicsObject *second;

  /**
   * Collision points.
   */
  std::vector<TContactPoint> contacts;
};
