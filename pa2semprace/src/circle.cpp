#include "circle.hpp"
#include "rectangle.hpp"
#include "complexObject.hpp"


using namespace std;
using namespace collision;

CCircle::CCircle( TVector<2> centre, double size, double density )
        : CPhysicsObject( centre,
                          TPhysicsAttributes::circleAttributes( density, size ) ),
          m_radius( size )
{
  m_boundingRadius = size;
}

void CCircle::render( CWindow &win ) const
{
  win.drawCircle( m_position, m_radius, m_rotation, m_tag );
}

TManifold CCircle::getManifold( CPhysicsObject *other )
{
  return other->getManifold( this );
}

TManifold CCircle::getManifold( CRectangle *line )
{
  return line->getManifold( this );
}

TManifold CCircle::getManifold( CCircle *other )
{
  TContactPoint contact = circleCircle( m_position, m_radius,
                                        other->m_position, other->m_radius );
  if( !contact.contactPoint )
    return { nullptr, nullptr };
  return { this, other, contact };
}

TManifold CCircle::getManifold( CComplexObject *other )
{
  return other->getManifold( this );
}

CPhysicsObject &CCircle::rotate( double angle )
{
  return CPhysicsObject::rotate( angle );
}

double CCircle::rayTrace( const TVector<2> &position, const TVector<2> &direction ) const
{
  if( CPhysicsObject::rayTrace( position, direction ) == HUGE_VAL )
    return HUGE_VAL;
  return rayTrace( position, direction, m_position, m_radius );
}

double CCircle::rayTrace( const TVector<2> &position,
                          const TVector<2> &direction,
                          const TVector<2> &centre,
                          double radius )
{
  TVector<2> unit = direction.normalized();
  TVector<2> chordHeight = ( centre - position ).rejectedFrom( unit );
  if( chordHeight.squareNorm() > radius * radius )
    return HUGE_VAL;

  TVector<2> chordCentre = ( centre - position ).projectedTo( unit );
  double chordHalfLength = sqrt( radius * radius - chordHeight.squareNorm() );
  TVector<2> chordStart = chordCentre - unit.stretchedTo( chordHalfLength );
  TVector<2> chordEnd = chordCentre + unit.stretchedTo( chordHalfLength );

  double startDistance = unit.dot( chordStart );
  double endDistance = unit.dot( chordEnd );
  if( startDistance * endDistance <= 0 )
    return 0;
  if( startDistance < 0 )
    return HUGE_VAL;

  return min( startDistance, endDistance );
}
