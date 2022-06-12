#pragma once

#include "physicsObject.hpp"

/**
 * @class CCircle
 */
class CCircle : public CPhysicsObject
{
public:
  /**
   * Constructor of circle.
   * @param centre position of circle
   * @param radius radius of circle
   * @param density circle density
   */
  CCircle( TVector<2> centre, double radius, double density );

  /**
   * Renders circle to window.
   * @param window target window
   */
  void render( CWindow &window ) const override;

  /**
   * Method for performing rotation on circle.
   * @param angle angle to rotate object
   * @return CPhysicsObject instance.
   */
  CPhysicsObject &rotate( double angle ) override;

  /**
   * Calculates collision manifold with other object.
   * @param other physics object
   * @return Collision manifold.
   */
  TManifold getManifold( CPhysicsObject *other ) override;

  /**
   * Calculates collision manifold with rectangle.
   * @param rectangle
   * @return Collision manifold.
   */
  TManifold getManifold( CRectangle *rectangle ) override;

  /**
   * Calculates collision manifold with other circle.
   * @param other circle
   * @return Collision manifold.
   */
  TManifold getManifold( CCircle *other ) override;

  /**
   * Calculates collision manifold with complex object.
   * @param complexObject complex object
   * @return Collision manifold.
   */
  TManifold getManifold( CComplexObject *complexObject ) override;

  /**
   * Calculates largest distance ray can travel from position in direction.
   * @return Positive double.
   */
  [[nodiscard]] double rayTrace( const TVector<2> &position,
                                 const TVector<2> &direction ) const override;

  /**
   * Static ray trace for ray( position, direction ) and circle( centre, radius ).
   * @return Positive double.
   */
  [[nodiscard]] static double rayTrace( const TVector<2> &position,
                                        const TVector<2> &direction,
                                        const TVector<2> &centre,
                                        double radius );

  /**
   * Circle radius.
   */
  double m_radius;
};



