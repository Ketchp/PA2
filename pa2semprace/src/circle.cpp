#include "circle.hpp"
#include "rectangle.hpp"
#include "complexObject.hpp"

using namespace std;

CCircle::CCircle( int id, TVector<2> centre, double size, double density )
        : CPhysicsObject( id, centre,
                          TPhysicsAttributes::circleAttributes( density, size ),
                          0 ),
          m_radius( size ),
          id( id )
{}

void CCircle::render( const CWindow &win ) const
{
  win.drawCircle( m_position, m_radius );
  glColor3f( 0, 0, 0 );
  glTranslatef( 0, 0, 1 );
  win.drawLine( m_position,
                m_position + TVector<2>::canonical( 0, m_radius ).rotated( m_rotation ),
                2 );
  glTranslatef( 0, 0, -1 );
  glColor3f( 1, 1, 1 );
}

TManifold CCircle::getManifold( CObject *other )
{
  return other->getManifold( this );
}

TManifold CCircle::getManifold( CRectangle *line )
{
  return line->getManifold( this );
}

TManifold CCircle::getManifold( CCircle *other )
{
  TContactPoint contact = circleCircleCollision( m_position, m_radius,
                                                 other->m_position, other->m_radius );
  if( !contact.contactPoint )
    return { nullptr, nullptr };
  return { this, other, contact };
}

TManifold CCircle::getManifold( CComplexObject *other )
{
  return other->getManifold( this );
}

CObject &CCircle::rotate( double angle )
{
  m_rotation += angle;
  return *this;
}

double CCircle::rayTrace( const TVector<2> &position, const TVector<2> &direction ) const
{
  TVector<2> unit = direction.normalized();
  TVector<2> chordHeight = ( m_position - position ).rejectedFrom( unit );
  if( chordHeight.squareNorm() > m_radius * m_radius )
    return HUGE_VAL;

  TVector<2> chordCentre = ( m_position - position ).projectedTo( unit );
  double chordHalfLength = sqrt( m_radius * m_radius - chordHeight.squareNorm() );
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
