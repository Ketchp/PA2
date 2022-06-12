#pragma once

#include "linearAlgebra.hpp"
#include "physicsAttributes.hpp"
#include "manifold.hpp"
#include "forceField.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <numeric>
#include <set>


/**
 * Class for simulating physics.
 */
class CPhysicsEngine
{
public:
  /**
   * Does single time step with time delta dt on objects.
   * @param objects
   * @param dt
   * @return Vector of all collisions.
   */
  std::vector<TManifold> step( std::vector<CPhysicsObject *> &objects, double dt );

  /**
   * Adds field to engine.
   * @param field
   */
  void addField( CForceField field );

  /**
   * Resets engine. Erases all fields.
   */
  void reset();

  /**
   * Frames elapsed from last reset / start.
   */
  size_t frame = 0;
private:
  /**
   * Accumulates all forces acting on all objects.
   * @param objects
   */
  void accumulateForces( std::vector<CPhysicsObject *> &objects );

  /**
   * Applies accumulated forces to all objects.
   * @param objects
   * @param dt time step
   */
  static void applyForces( std::vector<CPhysicsObject *> &objects, double dt );

  /**
   * Finds all collisions.
   * @param objects
   * @return Vector of all collisions.
   */
  static std::vector<TManifold> findCollisions( std::vector<CPhysicsObject *> &objects );

  /**
   * Resolves collision by pushing ( m_position translation ) objects according to overlap vector.
   * @param collisions
   */
  static void resolveCollisions( std::vector<TManifold> &collisions );

  /**
   * Resolves single collision.
   * @param collision
   */
  static void resolveCollision( const TManifold &collision );

  /**
   * Resolves collision in single point.
   * @param first
   * @param second
   * @param overlapVector
   */
  static void resolveCollision( CPhysicsObject &first, CPhysicsObject &second,
                                const TVector<2> &overlapVector );

  /**
   * Applies impulses to all colliding objects ( velocity, angular velocity update )
   * @param manifolds
   */
  static void applyImpulses( std::vector<TManifold> &manifolds );

  /**
   * Applies impulse caused by single collision.
   * @param manifold
   */
  static void applyImpulse( const TManifold &manifold );

  /**
   * Applies impulse caused by single contact point.
   * @param first
   * @param second
   * @param contactPoint
   */
  static void applyImpulse( CPhysicsObject &first, CPhysicsObject &second,
                            const TContactPoint &contactPoint );

  /**
   * Calculates combined normal impulse caused by collision in single point.
   * @param first
   * @param second
   * @param contactPoint
   * @return
   */
  static TVector<2> getNormalImpulse( CPhysicsObject &first, CPhysicsObject &second,
                                      const TContactPoint &contactPoint );

  /**
   * Calculates combined friction impulse caused by collision in single point.
   * @param first
   * @param second
   * @param contactPoint
   * @return
   */
  static TVector<2> getFrictionImpulse( CPhysicsObject &first, CPhysicsObject &second,
                                        const TContactPoint &contactPoint );

  /**
   * Calculates relative velocity of object points in pointOfContact
   * @param first
   * @param second
   * @param pointOfContact
   * @return
   */
  static TVector<2> getRelativeVelocity( const CPhysicsObject &first,
                                         const CPhysicsObject &second,
                                         const TVector<2> &pointOfContact );

  /**
   * Harmonic sum of two objects inverted masses.
   * @param first
   * @param second
   * @param point
   * @param direction
   * @return
   */
  static double getCombinedInvMass( const CPhysicsObject &first,
                                    const CPhysicsObject &second,
                                    const TVector<2> &point,
                                    const TVector<2> &direction );

  /**
   * Calculates harmonic sum of object mass and angular mass.
   * Angular mass acts to oppose force in direction at point.
   * @param object
   * @param point
   * @param direction
   * @return
   */
  static double getObjectInvMass( const CPhysicsObject &object,
                                  const TVector<2> &point,
                                  const TVector<2> &direction );

  /**
   * Vector of field acting on objects.
   */
  std::vector<CForceField> m_fields;
};
