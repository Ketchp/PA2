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


class CPhysicsEngine
{
public:
  std::vector<TManifold> step( std::vector<CPhysicsObject *> &, double );
  void addField( CForceField );
  void reset();

  size_t frame = 0;
private:
  void accumulateForces( std::vector<CPhysicsObject *> & );
  static void applyForces( std::vector<CPhysicsObject *> &, double );
  static std::vector<TManifold> findCollisions( std::vector<CPhysicsObject *> & );
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
};
