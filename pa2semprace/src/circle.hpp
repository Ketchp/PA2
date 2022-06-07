#pragma once
#include "object.hpp"

class CCircle : public CPhysicsObject
{
public:
  CCircle( int, TVector<2>, double size, double );
  void render( CWindow & ) const override;
  CObject &rotate( double angle ) override;
  TManifold getManifold( CObject * ) override;
  TManifold getManifold( CRectangle * ) override;
  TManifold getManifold( CCircle * ) override;
  TManifold getManifold( CComplexObject * ) override;

  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override;

  double m_radius;
  int id;
};



