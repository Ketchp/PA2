#pragma once

#include "object.hpp"
#include "manifold.hpp"
#include "physicsAttributes.hpp"


class CRectangle;

class CCircle;

class CComplexObject;

/**
 * @class CRectangle
 */
class CPhysicsObject : public CObject
{
public:
  /**
   * Constructor.
   * @param position centre of mass position
   * @param attributes physics attributes
   * @param angle initial angle
   */
  CPhysicsObject( TVector<2> position, const TPhysicsAttributes &attributes, double angle = 0 );

  ~CPhysicsObject() override = default;

  void render( CWindow &window ) const override = 0;

  virtual TManifold getManifold( CPhysicsObject * ) = 0;

  virtual TManifold getManifold( CRectangle * ) = 0;

  virtual TManifold getManifold( CCircle * ) = 0;

  virtual TManifold getManifold( CComplexObject * ) = 0;

  /**
   * Rotates object.
   * @param angle
   * @return CPhysicsObject.
   */
  virtual CPhysicsObject &rotate( double angle );

  /**
   * Ray cast against object. CPhysics object is abstract and
   * facilitates ray cast check only for tag properties.
   * @param position ray origin
   * @param direction ray direction
   * @return HUGE_VAL if object is transparent
   * @return NAN otherwise
   */
  [[nodiscard]] virtual double rayTrace( const TVector<2> &position,
                                         const TVector<2> &direction ) const;

  /**
   * Resets all accumulators in object physics attributes.
   */
  void resetAccumulator();

  /**
   * Applies accumulated forces and torques to object.
   * @param dt time delta
   */
  void applyForce( double dt );

  /**
   * Applies impulse to object.
   * @param impulse impulse
   * @param point point of interaction.
   */
  void applyImpulse( const TVector<2> &impulse, const TVector<2> &point );

  /**
   * Calculates velocity ( translational + rotational ) of object at point.
   * @param point point of velocity
   * @return Velocity of object at point.
   */
  [[nodiscard]] TVector<2> getLocalVelocity( const TVector<2> &point ) const;

  /**
   * Physics attributes of object.
   */
  TPhysicsAttributes m_attributes;

  /**
   * Radius of bounding box centred at m_position.
   */
  double m_boundingRadius = HUGE_VAL;

  /**
   * Objects rotation.
   */
  double m_rotation;
};

namespace collision
{

/**
 * Calculates collision information between two circles.
 * @param centreA, centreB centre of circle
 * @param radiusA, radiusB radius of circle
 * @return TContactPoint between circles.
 */
TContactPoint circleCircle( const TVector<2> &centreA, double radiusA,
                            const TVector<2> &centreB, double radiusB );

/**
 *
 * @param position
 * @param sizes
 * @param rotation
 * @param centre
 * @param radius 
 * @return
 */
TContactPoint rectCircle( const TVector<2> &position,
                          const TVector<2> &sizes,
                          double rotation,
                          const TVector<2> &centre,
                          double radius );

TContactPoint rectRect( const TVector<2> &, const TVector<2> &, double,
                        const TVector<2> &, const TVector<2> &, double );

TContactPoint firstRectOverlap( const TVector<2> &, const TVector<2> &, double,
                                const TVector<2> &, const TVector<2> &, double );

TMatrix<2, 4> rectCorners( const TVector<2> &, const TVector<2> &, double );

template <size_t dim>
TContactPoint axisPointsPenetration( const TVector<2> &axisDirection,
                                     const TVector<2> &axisPoint,
                                     const TMatrix<2, dim> &points );

TVector<2> lineSegmentClosestPoint( const TVector<2> &,
                                    const TVector<2> &,
                                    const TVector<2> & );

double rayTraceLineSeg( const TVector<2> &position,
                        const TVector<2> &direction,
                        const TVector<2> &begin,
                        const TVector<2> &end );

} // namespace collision