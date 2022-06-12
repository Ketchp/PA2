#pragma once

#include "physicsObject.hpp"


class CRectangle : public CPhysicsObject
{
public:
  CRectangle( TVector<2> centrePoint,
              TVector<2> size,
              double rotation, double density );

  void render( CWindow & ) const override;

  CPhysicsObject &rotate( double angle ) override;

  TManifold getManifold( CPhysicsObject * ) override;

  TManifold getManifold( CRectangle * ) override;

  TManifold getManifold( CCircle * ) override;

  TManifold getManifold( CComplexObject * ) override;

  TVector<2> rectangleClosestPoint( const TVector<2> & ) const;

  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override;

  static double rayTrace( const TVector<2> &position,
                          const TVector<2> &direction,
                          const TMatrix<2, 4> &rectCorners );

  [[nodiscard]] TVector<2> left() const;

  [[nodiscard]] TVector<2> right() const;

  [[nodiscard]] TMatrix<2, 4> corners() const;


  // vector from object centre to top-right corner
  TVector<2> m_size;
};



