#pragma once
#include "physicsObject.hpp"

class CComplexObject : public CPhysicsObject
{
public:
  explicit CComplexObject( double width );
  CComplexObject( std::vector<TVector<2>> vertices, double width, double density );
  void spawn( double density = HUGE_VAL );
  void render( CWindow & ) const override;

  TManifold getManifold( CPhysicsObject * ) override;
  TManifold getManifold( CRectangle * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;

  CPhysicsObject &rotate( double angle ) override;
  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override;

  void addVertex( const TVector<2> & );
  static TVector<2> calculateCentreOfMass( const std::vector<TVector<2>> &vertices );

  double m_width;
private:
  std::vector<TVector<2>> m_vertices;
};

