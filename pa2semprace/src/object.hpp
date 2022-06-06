#pragma once
#include "linearAlgebra.hpp"
#include "physicsAttributes.hpp"
#include "physicsEngine.hpp"
#include "window.hpp"
#include "manifold.hpp"
#include "tags.hpp"
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <iostream>

class CObject;
class CPhysicsObject;
class CRectangle;
class CCircle;
class CComplexObject;


class CObject
{
public:
  explicit CObject( int id, TVector<2> position );
  virtual ~CObject() = default;
  virtual void render( const CWindow & ) const = 0;
  virtual CObject &rotate( double ) = 0;
  virtual void resetAccumulator();
  virtual void accumulateForce( const CForceField & );
  virtual void applyForce( double );
  virtual TManifold getManifold( CObject * ) = 0;
  virtual TManifold getManifold( CRectangle * ) = 0;
  virtual TManifold getManifold( CCircle * ) = 0;
  virtual TManifold getManifold( CComplexObject * ) = 0;

  virtual double rayTrace( const TVector<2> &position,
                           const TVector<2> &direction ) const;

  void addTag( ETag tag );

  int m_id;
  ETag tags = ETag::NONE;
  TVector<2> m_position;
};

class CPhysicsObject : public CObject
{
public:
  CPhysicsObject( int, TVector<2>, const TPhysicsAttributes &attributes, double );
  ~CPhysicsObject() override = default;
  void render( const CWindow & ) const override = 0;
  CObject &rotate( double ) override;
  void resetAccumulator() final;
  void accumulateForce( const CForceField & ) final;
  void applyForce( double ) final;
  void applyImpulse( const TVector<2> &, const TVector<2> & );
  TVector<2> getLocalVelocity( const TVector<2> & ) const;

  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override = 0;

  TPhysicsAttributes m_attributes;
  double m_rotation = 0;
};

TContactPoint lineLineCollision( const TVector<2> &, const TVector<2> &,
                                 const TVector<2> &, const TVector<2> & );

TContactPoint lineCircleCollision( const TVector<2> &, const TVector<2> &,
                                   const TVector<2> &, double );

TContactPoint circleCircleCollision( const TVector<2> &position, double radius,
                                     const TVector<2> &, double );

TContactPoint rectCircleCollision( const TVector<2> &position,
                                   const TVector<2> &sizes,
                                   double rotation,
                                   const TVector<2> &, double );

TContactPoint rectRectCollision( const TVector<2> &, const TVector<2> &, double,
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

TContactPoint biggestOverlap( CObject *, const std::vector<CObject *> & );

double rayTraceLineSeg( const TVector<2> &position,
                        const TVector<2> &direction,
                        const TVector<2> &begin,
                        const TVector<2> &end );