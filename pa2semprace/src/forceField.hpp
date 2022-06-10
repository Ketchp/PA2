#pragma once
#include "physicsObject.hpp"

class CForceField
{
public:
  explicit CForceField( std::function<void(CPhysicsObject &)> );
  void applyForce( CPhysicsObject & ) const;
  static CForceField gravitationalField( double g = 50 );
  std::function<void(CPhysicsObject &)> m_fieldFunctor;
};

