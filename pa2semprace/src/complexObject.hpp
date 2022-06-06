#pragma once
#include "object.hpp"

class CComplexObject : public CPhysicsObject
{
public:
  CComplexObject( int id, double width );
  CComplexObject( int, std::vector<TVector<2>> vertices, double width, double density );
  void spawn( double density = NAN );
  void render( const CWindow & ) const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CRectangle * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;
  static TVector<2> calculateCentreOfMass( const std::vector<TVector<2>> &vertices );
  void addVertex( const TVector<2> & );

  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override;

  double m_width;
private:
  std::vector<TVector<2>> m_vertices;
};

