#pragma once

#include "object.hpp"
#include "manifold.hpp"
#include "physicsAttributes.hpp"


class CRectangle;

class CCircle;

class CComplexObject;

/**
 * Base class for all objects that are part of physics simulator.
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

  /**
   * Calculates collision manifold with other object.
   * @param other physics object
   * @return Collision manifold.
   */
  virtual TManifold getManifold( CPhysicsObject *other ) = 0;

  /**
   * Calculates collision manifold with rectangle.
   * @param rectangle physics object
   * @return Collision manifold.
   */
  virtual TManifold getManifold( CRectangle *rectangle ) = 0;

  /**
   * Calculates collision manifold with circle.
   * @param circle
   * @return Collision manifold.
   */
  virtual TManifold getManifold( CCircle *circle ) = 0;

  /**
   * Calculates collision manifold with complex object.
   * @param complexObject
   * @return Collision manifold.
   */
  virtual TManifold getManifold( CComplexObject *complexObject ) = 0;

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
 * Calculates collision information between rectangle and circle.
 * @param position centre of rectangle
 * @param size rectangle size
 * @param rotation rectangle rotation
 * @param centre centre of circle
 * @param radius circle radius
 * @return TContactPoint between rectangle and circle.
 */
TContactPoint rectCircle( const TVector<2> &position,
                          const TVector<2> &size,
                          double rotation,
                          const TVector<2> &centre,
                          double radius );

/**
 * Calculates collision information between two rectangles.
 * @param positionA, positionB rectangle centre
 * @param sizeA, sizeB rectangle size
 * @param rotationA, rotationB rectangle rotation
 * @return TContactPoint between two rectangles.
 */
TContactPoint rectRect( const TVector<2> &positionA, const TVector<2> &sizeA, double rotationA,
                        const TVector<2> &positionB, const TVector<2> &sizeB, double rotationB );

/**
 * Calculates collision information between two rectangles.
 * Collision overlap is calculated only in direction parallel to first rectangle sides.
 * @param positionA, positionB rectangle centre
 * @param sizeA, sizeB rectangle size
 * @param rotationA, rotationB rectangle rotation
 * @return TContactPoint between two rectangles.
 */
TContactPoint firstRectOverlap( const TVector<2> &positionA, const TVector<2> &sizeA, double rotationA,
                                const TVector<2> &positionB, const TVector<2> &sizeB, double rotationB );

/**
 * @param position rectangle centre
 * @param size rectangle size
 * @param rotation rectangle rotation
 * @return Matrix of rectangle corners from top-right clockwise.
 */
TMatrix<2, 4> rectCorners( const TVector<2> &position, const TVector<2> &size, double rotation );

/**
 * Calculates most-left overlap of points through axis.
 * @tparam dim point count
 * @param axisDirection direction of axis
 * @param axisPoint any point on axis
 * @param points points
 * @return Overlap of most-left point.
 */
template <size_t dim>
TContactPoint axisPointsPenetration( const TVector<2> &axisDirection,
                                     const TVector<2> &axisPoint,
                                     const TMatrix<2, dim> &points );

/**
 * Calculates closest point on line segment to point.
 * @param begin begin of line segment
 * @param end end of line segment
 * @param point test point
 * @return Closest point on segment to test point.
 */
TVector<2> lineSegmentClosestPoint( const TVector<2> &begin,
                                    const TVector<2> &end,
                                    const TVector<2> &point );

/**
 * Calculates ray length before hitting line segment.
 * @param position ray origin
 * @param direction ray direction
 * @param begin line segment begin
 * @param end line segment end
 * @return Length ray can travel before hitting line segment.
 * Negative number if ray hits segment in opposite direction.
 * HUGE_VAL if ray never hits line segment.
 */
double rayTraceLineSeg( const TVector<2> &position,
                        const TVector<2> &direction,
                        const TVector<2> &begin,
                        const TVector<2> &end );

} // namespace collision