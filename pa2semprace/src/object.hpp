#pragma once
#include <GL/freeglut.h>
#include "physicsAttributes.hpp"
#include <string>
#include "linearAlgebra.hpp"
#include "helpers.hpp"
#include "matrix.hpp"
#include "physicsEngine.hpp"
#include <vector>

struct CManifold;

class CObject
{
public:
  explicit CObject( math::vector position, double rotation = 0 );
  virtual ~CObject() = default;
  virtual void render() const = 0;
  virtual void updateBoundingBox() = 0;
  virtual CManifold getManifold( CObject * ) = 0;
  math::vector m_position;
  math::vector m_BBx;
  virtual CObject &rotate( double angle );
protected:
  double m_rotation;
};

class CPhysicsObject : public CObject
{
public:
  CPhysicsObject( math::vector position, double rotation, const physicsAttributes &attributes );
  CPhysicsObject( math::vector position, const physicsAttributes &attributes );
  ~CPhysicsObject() override = default;
  void render() const override = 0;
  void updateBoundingBox() override = 0;
  CManifold getManifold( CObject * ) override = 0;
  physicsAttributes m_attributes;
  CObject &rotate( double angle ) override = 0;
};

class CLine : public CPhysicsObject
{
public:
  CLine( math::vector pointStart, math::vector pointEnd, const physicsAttributes & );
  void render() const override;
  void updateBoundingBox() override;
  CManifold getManifold( CObject * ) override;
  CManifold getManifold( CLine * );
  [[nodiscard]] math::vector begin() const;
  [[nodiscard]] math::vector end() const;
  static math::vector calculateCollisionPoint( math::vector,
                                               math::vector,
                                               math::vector,
                                               math::vector );
  static bool areColliding( const math::vector &,
                            const math::vector &,
                            const math::vector &,
                            const math::vector & );

  CObject &rotate( double angle ) override;
  math::vector m_direction;
};

class CCircle : public CPhysicsObject
{
public:
  CCircle( math::vector, double size, const physicsAttributes & );
  void render() const override;
  void updateBoundingBox() override;
  CManifold getManifold( CObject * ) override;
  CManifold getManifold( CLine * );
  CManifold getManifold( CCircle * );
  CObject &rotate( double angle ) override;
  double m_size;
  static GLUquadric *gluRenderer;
};

class CComplexObject : public CPhysicsObject
{
public:
  CComplexObject( std::vector<math::vector> vertices, const physicsAttributes & );
  void render() const override;
  void updateBoundingBox() override;
  CManifold getManifold( CObject * ) override;
  CManifold getManifold( CLine * );
  CManifold getManifold( CCircle * );
  CManifold getManifold( CComplexObject * );
  CObject &rotate( double angle ) override;
  static math::vector calculateCentreOfMass( const std::vector<math::vector> &vertices );
  static CComplexObject makeRectangle( math::vector centre, math::vector size, const physicsAttributes & );
  std::vector<math::vector> m_vertices;
};

