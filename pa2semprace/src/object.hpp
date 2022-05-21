#pragma once
#include "physicsAttributes.hpp"
#include "linearAlgebra.hpp"
#include "physicsEngine.hpp"
#include "helpers.hpp"
#include <GL/freeglut.h>
#include <string>
#include <vector>

class CObject;
class CPhysicsObject;
class CLine;
class CCircle;
class CComplexObject;


class CObject
{
public:
  explicit CObject( int id, TVector<2> position );
  virtual ~CObject() = default;
  virtual void render() const = 0;
  virtual CObject &rotate( double );
  virtual CObject &translate( TVector<2> );
  virtual void accumulateForce( const CForceField & );
  virtual void applyForce();
  virtual TManifold getManifold( CObject * ) = 0;
  virtual TManifold getManifold( CLine * ) = 0;
  virtual TManifold getManifold( CCircle * ) = 0;
  virtual TManifold getManifold( CComplexObject * ) = 0;

  int m_id;
  TVector<2> m_position;
};

class CPhysicsObject : public CObject
{
public:
  CPhysicsObject( int, TVector<2>, const TPhysicsAttributes &attributes );
  ~CPhysicsObject() override = default;
  CObject &rotate( double angle ) override = 0;
  void applyForce() override;
  TPhysicsAttributes m_attributes;
};

class CLine : public CPhysicsObject
{
public:
  CLine( int, TVector<2> pointStart, TVector<2> pointEnd, double );
  void render() const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CLine * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;
  [[nodiscard]] TVector<2> begin() const;
  [[nodiscard]] TVector<2> end() const;
  static TVector<2> calculateCollisionPoint( TVector<2>,
                                             TVector<2>,
                                             TVector<2>,
                                             TVector<2> );
  TVector<2> m_direction;
};

class CCircle : public CPhysicsObject
{
public:
  CCircle( int, TVector<2>, double size, double );
  void render() const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CLine * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;
  double m_size;
  static GLUquadric *gluRenderer;
};

class CComplexObject : public CPhysicsObject
{
public:
  CComplexObject( int, std::vector<TVector<2>> vertices, double );
  void render() const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CLine * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;
  static TVector<2> calculateCentreOfMass( const std::vector<TVector<2>> &vertices );
  [[nodiscard]] static CComplexObject *makeRectangle( int, TVector<2> centre, TVector<2> size, double );
  std::vector<TVector<2>> m_vertices;
};

