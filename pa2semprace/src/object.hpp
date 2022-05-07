#pragma once
#include <GL/freeglut.h>
#include "physicsAttributes.hpp"
#include <string>
#include <vector>
#include "physicsEngine.hpp"
#include "helpers.hpp"
#include "vector.hpp"
#include "matrix.hpp"

class CObject
{
public:
  explicit CObject( math::vector position, double rotation = 0 );
  virtual ~CObject() = default;
  virtual void render() const = 0;
  virtual void updateBoundingBox() = 0;
  virtual CManifold getManifold( CObject * ) = 0;
  math::vector m_position;
  double m_rotation;
  math::vector m_BBx;
};

class CPhysicsObject : public CObject
{
public:
  CPhysicsObject( math::vector position, double rotation, const physicsAttributes &attributes );
  CPhysicsObject( math::vector position, const physicsAttributes &attributes );
  ~CPhysicsObject() override = default;
  void render() const override;
  void updateBoundingBox() override = 0;
  CManifold getManifold( CObject * ) override = 0;
  physicsAttributes m_attributes;
};

class CLine : public CPhysicsObject
{
public:
  CLine( math::vector pointStart, math::vector pointEnd, const physicsAttributes & );
  void render() const override;
  void updateBoundingBox() override;
  CManifold getManifold( CObject * ) override;
  CManifold getManifold( CLine * );
  math::vector begin() const;
  math::vector end() const;
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
  static math::vector calculateCentreOfMass( const std::vector<math::vector> &vertices );
  static CComplexObject makeRectangle( math::vector centre, math::vector size, const physicsAttributes & );
  std::vector<math::vector> m_vertices;
};

