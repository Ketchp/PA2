#pragma once

#include "physicsObject.hpp"


class CCircle : public CPhysicsObject
{
public:
  CCircle( TVector<2>, double size, double density );

  void render( CWindow & ) const override;

  CPhysicsObject &rotate( double angle ) override;

  TManifold getManifold( CPhysicsObject * ) override;

  TManifold getManifold( CRectangle * ) override;

  TManifold getManifold( CCircle * ) override;

  TManifold getManifold( CComplexObject * ) override;

  [[nodiscard]] double rayTrace( const TVector<2> &position,
                                 const TVector<2> &direction ) const override;

  [[nodiscard]] static double rayTrace( const TVector<2> &position,
                                        const TVector<2> &direction,
                                        const TVector<2> &centre,
                                        double radius );

  double m_radius;
};



