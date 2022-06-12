#include "rectangle.hpp"
#include "circle.hpp"
#include "complexObject.hpp"


using namespace std;
using namespace collision;

CRectangle::CRectangle( TVector<2> centrePoint,
                        TVector<2> size,
                        double rotation, double density )
        : CPhysicsObject( centrePoint,
                          TPhysicsAttributes::rectangleAttributes( density,
                                                                   size ),
                          rotation ),
          m_size{ size / 2 }
{
  m_boundingRadius = m_size.norm();
}

void CRectangle::render( CWindow &win ) const
{
  win.drawLine( left(), right(), m_size[ 1 ], m_tag );
}

TManifold CRectangle::getManifold( CPhysicsObject *other )
{
  return other->getManifold( this );
}

TManifold CRectangle::getManifold( CRectangle *other )
{
  TContactPoint contact = rectRect( m_position, m_size, m_rotation,
                                    other->m_position, other->m_size, other->m_rotation );
  if( !contact.contactPoint )
    return { nullptr, nullptr };
  return { this, other, contact };
}

TManifold CRectangle::getManifold( CCircle *circle )
{
  TContactPoint contact = rectCircle( m_position, m_size, m_rotation,
                                      circle->m_position, circle->m_radius );

  if( !contact.contactPoint )
    return { nullptr, nullptr };

  return { this, circle, contact };
}

TManifold CRectangle::getManifold( CComplexObject *other )
{
  return other->getManifold( this );
}

CPhysicsObject &CRectangle::rotate( double angle )
{
  return CPhysicsObject::rotate( angle );
}

TVector<2> CRectangle::left() const
{
  TVector<2> dir = m_size;
  dir[ 1 ] = 0;
  dir.rotate( m_rotation );
  return m_position - dir;
}

TVector<2> CRectangle::right() const
{
  TVector<2> dir = m_size;
  dir[ 1 ] = 0;
  dir.rotate( m_rotation );
  return m_position + dir;
}

TMatrix<2, 4> CRectangle::corners() const
{
  return rectCorners( m_position, m_size, m_rotation );
}

TVector<2> CRectangle::rectangleClosestPoint( const TVector<2> &point ) const
{
  TMatrix<2, 4> corn = corners();
  double smallestDist = HUGE_VAL;
  TVector<2> closest;
  for( size_t idx = 0; idx < 4; ++idx )
  {
    TVector<2> temp = lineSegmentClosestPoint( corn[ idx ],
                                               corn[ ( idx + 1 ) % 4 ],
                                               point );
    double newDist = temp.squareDistance( point );
    if( newDist >= smallestDist )
      continue;
    smallestDist = newDist;
    closest = temp;
  }
  return closest;
}

double CRectangle::rayTrace( const TVector<2> &position, const TVector<2> &direction ) const
{
  if( CPhysicsObject::rayTrace( position, direction ) == HUGE_VAL )
    return HUGE_VAL;
  return rayTrace( position, direction, corners() );
}

double CRectangle::rayTrace( const TVector<2> &position,
                             const TVector<2> &direction,
                             const TMatrix<2, 4> &rectCorners )
{
  double min = HUGE_VAL;
  bool negative = false;
  for( size_t idx = 0; idx < 4; ++idx )
  {
    double rayLen = rayTraceLineSeg( position, direction,
                                     rectCorners[ idx ],
                                     rectCorners[ ( idx + 1 ) % 4 ] );
    if( rayLen <= 0 )
      negative = true;
    else if( rayLen < min )
      min = rayLen;
  }
  if( negative && min != HUGE_VAL )
    return 0;
  return min;
}
