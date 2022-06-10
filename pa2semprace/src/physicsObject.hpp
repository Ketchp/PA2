#pragma once
#include "object.hpp"
#include "manifold.hpp"
#include "physicsAttributes.hpp"

class CRectangle;
class CCircle;
class CComplexObject;

class CPhysicsObject : public CObject
{
public:
  CPhysicsObject( TVector<2>, const TPhysicsAttributes &attributes, double = 0 );
  ~CPhysicsObject() override = default;

  void render( CWindow & ) const override = 0;
  virtual TManifold getManifold( CPhysicsObject * ) = 0;
  virtual TManifold getManifold( CRectangle * ) = 0;
  virtual TManifold getManifold( CCircle * ) = 0;
  virtual TManifold getManifold( CComplexObject * ) = 0;

  virtual CPhysicsObject &rotate( double );
  virtual double rayTrace( const TVector<2> &position,
                           const TVector<2> &direction ) const;

  void resetAccumulator();
  void applyForce( double );
  void applyImpulse( const TVector<2> &, const TVector<2> & );
  [[nodiscard]] TVector<2> getLocalVelocity( const TVector<2> & ) const;

  TPhysicsAttributes m_attributes;
  double m_rotation;
  double m_boundingRadius = HUGE_VAL;
};

namespace collision
{
TContactPoint lineLine( const TVector<2> &, const TVector<2> &,
                        const TVector<2> &, const TVector<2> & );

TContactPoint lineCircle( const TVector<2> &, const TVector<2> &,
                          const TVector<2> &, double );

TContactPoint circleCircle( const TVector<2> &position, double radius,
                            const TVector<2> &, double );

TContactPoint rectCircle( const TVector<2> &position,
                          const TVector<2> &sizes,
                          double rotation,
                          const TVector<2> &, double );

TContactPoint rectRect( const TVector<2> &, const TVector<2> &, double,
                        const TVector<2> &, const TVector<2> &, double );

TContactPoint firstRectOverlap( const TVector<2> &, const TVector<2> &, double,
                                const TVector<2> &, const TVector<2> &, double );

TMatrix<2, 4> rectCorners( const TVector<2> &, const TVector<2> &, double );

double separation( const TVector<2> &axis,
                   const TMatrix<2, 4> &firstPoints,
                   const TMatrix<2, 4> &secondPoints );

template <size_t dim>
TContactPoint axisPointsPenetration( const TVector<2> &axisDirection,
                                     const TVector<2> &axisPoint,
                                     const TMatrix<2, dim> &points );

TVector<2> lineSegmentClosestPoint( const TVector<2> &,
                                    const TVector<2> &,
                                    const TVector<2> & );

TContactPoint biggestOverlap( CPhysicsObject *, const std::vector<CPhysicsObject *> & );

double rayTraceLineSeg( const TVector<2> &position,
                        const TVector<2> &direction,
                        const TVector<2> &begin,
                        const TVector<2> &end );

} // namespace collision