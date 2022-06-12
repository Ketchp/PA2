#pragma once

#include "physicsObject.hpp"

/**
 * Class for representing forces.
 */
class CForceField
{
public:
  /**
   * Creates force field.
   * @param function Function that applies force to physics object.
   */
  explicit CForceField( std::function<void( CPhysicsObject &object )> function );

  /**
   * Applies force field to object.
   * @param object Object to apply force to.
   */
  void applyForce( CPhysicsObject &object ) const;

  /**
   * Returns gravitational field with acceleration g.
   * @param g acceleration
   * @return CForceField global homogenous gravitational field.
   */
  static CForceField gravitationalField( double g = 50 );

  /**
   * Field function/functor.
   */
  std::function<void( CPhysicsObject & )> m_fieldFunctor;
};
