#pragma once
#include "linearAlgebra.hpp"
#include "physicsAttributes.hpp"
#include "physicsEngine.hpp"
#include "helpers.hpp"
#include "window.hpp"
#include "manifold.hpp"
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <iostream>

class CObject;
class CPhysicsObject;
class CRectangle;
class CCircle;
class CComplexObject;

class CWindow;


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

  int m_id;
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
  TPhysicsAttributes m_attributes;
  double m_rotation = 0;
};

class CRectangle : public CPhysicsObject
{
public:
  CRectangle( int, TVector<2> centrePoint,
              double width, double height,
              double rotation, double density );
  void render( const CWindow & ) const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CRectangle * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;

  TManifold rectOverlap( const CRectangle * ) const;
  TVector<2> rectangleClosestPoint( const TVector<2> & ) const;
  static TVector<2> lineSegmentClosestPoint( const TVector<2> &,
                                             const TVector<2> &,
                                             const TVector<2> &);

  [[nodiscard]] TVector<2> left() const;
  [[nodiscard]] TVector<2> right() const;

  [[nodiscard]] TMatrix<2, 4> corners() const;


  // vector from object centre to top-left corner
  TVector<2> m_direction;
  int id;
};

class CCircle : public CPhysicsObject
{
public:
  CCircle( int, TVector<2>, double size, double );
  void render( const CWindow & ) const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CRectangle * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;
  double m_radius;
  int id;
};

class CComplexObject : public CPhysicsObject
{
public:
  explicit CComplexObject();
  CComplexObject( int, std::vector<TVector<2>> vertices, double );
  void render( const CWindow & ) const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CRectangle * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;
  static TVector<2> calculateCentreOfMass( const std::vector<TVector<2>> &vertices );
  void addVertex( const TVector<2> & );
  std::vector<TVector<2>> m_vertices;
};

TVector<2> lineLineCollision( const TVector<2> &, const TVector<2> &,
                              const TVector<2> &, const TVector<2> & );
TVector<2> lineCircleCollision( const TVector<2> &, const TVector<2> &,
                                const TVector<2> &, double );
TVector<2> circleCircleCollision( const TVector<2> &, double,
                                  const TVector<2> &, double );

double separation( const TVector<2> &axis,
                   const TMatrix<2, 4> &firstPoints,
                   const TMatrix<2, 4> &secondPoints );

template <size_t dim>
TContactPoint axisPointsPenetration( const TVector<2> &axisDirection,
                                     const TVector<2> &axisPoint,
                                     const TMatrix<2, dim> &points );
