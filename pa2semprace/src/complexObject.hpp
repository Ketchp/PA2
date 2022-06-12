#pragma once

#include "physicsObject.hpp"


class CComplexObject : public CPhysicsObject
{
public:
  explicit CComplexObject( double width );

  void spawn( double density = HUGE_VAL );

  void render( CWindow & ) const override;

  TManifold getManifold( CPhysicsObject * ) override;

  TManifold getManifold( CRectangle * ) override;

  TManifold getManifold( CCircle * ) override;

  TManifold getManifold( CComplexObject * ) override;

  std::vector<TContactPoint> getCircleCollision( const TVector<2> &centre,
                                                 double radius ) const;

  std::vector<TContactPoint> getNodeCollisions( CComplexObject *,
                                                const TVector<2> &node ) const;

  CPhysicsObject &rotate( double angle ) override;

  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override;

  void addVertex( const TVector<2> & );

  static TVector<2> calculateCentreOfMass( const std::vector<TVector<2>> &vertices );

  double m_width;
  double m_longest = 0;
private:
  std::vector<TVector<2>> m_vertices;
};

