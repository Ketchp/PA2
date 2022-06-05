#pragma once
#include "object.hpp"

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

  TVector<2> rectangleClosestPoint( const TVector<2> & ) const;

  double rayTrace( const TVector<2> &position,
                   const TVector<2> &direction ) const override;

  [[nodiscard]] TVector<2> left() const;
  [[nodiscard]] TVector<2> right() const;

  [[nodiscard]] TMatrix<2, 4> corners() const;


  // vector from object centre to top-left corner
  TVector<2> m_size;
  int id;
};



