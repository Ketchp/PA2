#pragma once
#include "linearAlgebra.hpp"
#include "physicsAttributes.hpp"
#include "manifold.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <numeric>
#include <set>

class CForceField
{
public:
  explicit CForceField( std::function<void(CPhysicsObject &)> );
  void applyForce( CPhysicsObject & ) const;
  static CForceField gravitationalField( double g = 50 );
  std::function<void(CPhysicsObject &)> m_fieldFunctor;
};


class CPhysicsEngine
{
public:
  void step( std::vector<CObject *> &, double );
  void addField( CForceField );
  void reset();
  void registerCollisionCallback( const std::function<bool(const std::vector<TManifold> &)> &callback );

  size_t frame = 0;
private:
  void accumulateForces( std::vector<CObject *> & );
  static void applyForces( std::vector<CObject *> &, double );
  static std::vector<TManifold> findCollisions( std::vector<CObject *> & );
  static void resolveCollisions( std::vector<TManifold> & );
  static void resolveCollision( const TManifold & );
  static void resolveCollision( CPhysicsObject &, CPhysicsObject &,
                                const TContactPoint & );
  static void applyImpulses( std::vector<TManifold> & );
  static void applyImpulse( const TManifold & );
  static void applyImpulse( CPhysicsObject &, CPhysicsObject &,
                            const TContactPoint & );
  static TVector<2> getNormalImpulse( CPhysicsObject &, CPhysicsObject &,
                                      const TContactPoint & );
  static TVector<2> getFrictionImpulse( CPhysicsObject &, CPhysicsObject &,
                                        const TContactPoint & );
  static TVector<2> getRelativeVelocity( const CPhysicsObject &,
                                         const CPhysicsObject &,
                                         const TVector<2> &pointOfContact );
  static double getCombinedInvMass( const CPhysicsObject &,
                                    const CPhysicsObject &,
                                    const TVector<2> &point,
                                    const TVector<2> &direction );

  static double getObjectInvMass( const CPhysicsObject &,
                                  const TVector<2> &point,
                                  const TVector<2> &direction );

  std::vector<CForceField> m_fields;
  std::function<bool(const std::vector<TManifold> &)> m_collisionCallback;
};
