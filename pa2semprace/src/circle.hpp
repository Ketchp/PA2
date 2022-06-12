#pragma once

#include "physicsObject.hpp"

/**
 * @class CCircle
 */
class CCircle : public CPhysicsObject
{
public:
  /**
   * Constructor of circle
   * @param centre position of circle centre
   * @param radius radius of circle
   * @param density circle density
   */
  CCircle( TVector<2> centre, double radius, double density );

  /**
   * Renders circle to window
   * @param window target window
   */
  void render( CWindow &window ) const override;

  /**
   * method for performing rotation on circle
   * @param angle angle to rotate object
   * @return CPhysicsObject instance
   */
  CPhysicsObject &rotate( double angle ) override;

  TManifold getManifold( CPhysicsObject * ) override;

  TManifold getManifold( CRectangle * ) override;

  TManifold getManifold( CCircle * ) override;

  TManifold getManifold( CComplexObject * ) override;

  [[nodiscard]] double rayTrace( const TVector<2> &position,
                                 const TVector<2> &direction ) const override;

  [[nodiscard]] static double rayTrace( const TVector<2> &position,
                                        const TVector<2> &direction,
                                        const TVector<2> &centre,
                                        double radius );

  double m_radius;
};



