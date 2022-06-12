#pragma once

#include "physicsObject.hpp"

/**
 * Class for representation of physics rectangle.
 */
class CRectangle : public CPhysicsObject
{
public:
  /**
   * Constructs rectangle with centre, size, rotation, density.
   * @param centrePoint
   * @param size vector from centre to top-right corner
   * @param rotation
   * @param density
   */
  CRectangle( TVector<2> centrePoint,
              TVector<2> size,
              double rotation,
              double density );

  /**
   * Renders rectangle to window.
   * @param window target window
   */
  void render( CWindow &window ) const override;

  /**
   * Method for performing rotation on rectangle.
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
   * @param circle circle
   * @return Collision manifold.
   */
  TManifold getManifold( CCircle *circle ) override;

  /**
   * Calculates collision manifold with complex object.
   * @param complexObject complex object
   * @return Collision manifold.
   */
  TManifold getManifold( CComplexObject *complexObject ) override;

  /**
   * Calculates largest distance ray can travel from position in direction.
   * @param position
   * @param direction
   * @return Non-negative double.
   */
  [[nodiscard]] double rayTrace( const TVector<2> &position,
                                 const TVector<2> &direction ) const override;

   /**
    * Calculates largest distance ray can travel from position in direction,
    * until it hits quadrangle with corners.
    * @param position
    * @param direction
    * @param rectCorners
    * @return
    */
  static double rayTrace( const TVector<2> &position,
                          const TVector<2> &direction,
                          const TMatrix<2, 4> &rectCorners );

  /**
   * @return Left-most point.
   */
  [[nodiscard]] TVector<2> left() const;

  /**
   * @return Right-most point.
   */
  [[nodiscard]] TVector<2> right() const;

  /**
   * @return Corners of rectangle.
   */
  [[nodiscard]] TMatrix<2, 4> corners() const;


  /**
   * vector from object centre to top-right corner
   */
  TVector<2> m_size;
};



