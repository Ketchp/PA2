#include "complexObject.hpp"
#include "rectangle.hpp"
#include "circle.hpp"

using namespace std;


CComplexObject::CComplexObject( int id, double width )
  : CPhysicsObject( id, {}, { HUGE_VAL, HUGE_VAL }, 0 ),
    m_width( width )
{}

CComplexObject::CComplexObject( int id, vector<TVector<2>> vertices, double width, double density )
  : CPhysicsObject( id, calculateCentreOfMass( vertices ),
                    TPhysicsAttributes::complexObjectAttributes( width, density, vertices,
                                                                 calculateCentreOfMass( vertices ) ),
                    0 ),
    m_width( width ),
    m_vertices( move( vertices ) )
{
  for( auto &vertex: m_vertices )
    vertex -= m_position;
}

void CComplexObject::render( CWindow &win ) const
{
  if( m_vertices.empty() )
    return;
  win.drawCircle( m_position + m_vertices[ 0 ], m_width );
  for( size_t idx = 1; idx < m_vertices.size(); ++idx )
  {
    win.drawLine( m_position + m_vertices[ idx - 1 ], m_position + m_vertices[ idx ], m_width );
    win.drawCircle( m_position + m_vertices[ idx ], m_width );
  }
}

TVector<2> CComplexObject::calculateCentreOfMass( const vector<TVector<2>> &vertices )
{
  if( vertices.empty() )
    return {};
  else if( vertices.size() == 1 )
    return vertices[ 0 ];
  double weight = 0;
  TVector<2> weightedPosition;
  for( size_t idx = 1; idx < vertices.size(); ++idx )
  {
    TVector<2> direction = vertices[ idx ] - vertices[ idx - 1 ];
    TVector<2> position = vertices[ idx ] + vertices[ idx - 1 ]; // *2
    weight += direction.norm();
    weightedPosition += direction.norm() * position;
  }
  return weightedPosition / weight / 2;
}

TManifold CComplexObject::getManifold( CObject *other )
{
  return other->getManifold( this );
}

TManifold CComplexObject::getManifold( CRectangle *rect )
{
  if( m_vertices.empty() )
    return { nullptr, nullptr };

  vector<TContactPoint> contacts;

  for( const auto &vertex: m_vertices )
  {
    TContactPoint contact = rectCircleCollision( rect->m_position,
                                                 rect->m_size,
                                                 rect->m_rotation,
                                                 vertex + m_position, m_width );
    if( !contact.contactPoint )
      continue;
    contacts.push_back( contact );
  }

  for( auto it = m_vertices.begin() + 1;
       it != m_vertices.end(); ++it )
  {
    const auto &begin = *( it - 1 );
    const auto &end = *it;
    const auto direction = end - begin;
    TContactPoint contact = rectRectCollision( rect->m_position,
                                               rect->m_size,
                                               rect->m_rotation,
                                               m_position + ( begin + end ) / 2,
                                               { direction.norm() / 2, m_width },
                                               direction.getAngle() );
    if( !contact.contactPoint )
      continue;
    contacts.push_back( contact );
  }

  if( contacts.empty() )
    return { nullptr, nullptr };
  return { rect, this, contacts };
}

TManifold CComplexObject::getManifold( CCircle *circle )
{
  if( m_vertices.empty() )
    return { nullptr, nullptr };

  vector<TContactPoint> contacts;

  for( const auto &vertex: m_vertices )
  {
    TContactPoint contact = circleCircleCollision( circle->m_position,
                                                   circle->m_radius,
                                                   vertex + m_position,
                                                   m_width );
    if( !contact.contactPoint )
      continue;
    contacts.push_back( contact );
  }

  for( auto it = m_vertices.begin() + 1;
       it != m_vertices.end(); ++it )
  {
    const auto &begin = *( it - 1 );
    const auto &end = *it;
    const auto direction = end - begin;
    TContactPoint contact = rectCircleCollision( m_position + ( begin + end ) / 2,
                                                 { direction.norm() / 2, m_width },
                                                 direction.getAngle(),
                                                 circle->m_position,
                                                 circle->m_radius );
    if( !contact.contactPoint )
      continue;
    contact.overlapVector *= -1;
    contacts.push_back( contact );
  }

  if( contacts.empty() )
    return { nullptr, nullptr };
  return { circle, this, contacts };
}

TManifold CComplexObject::getManifold( CComplexObject *other )
{
  return { nullptr, nullptr }; // todo
}

CObject &CComplexObject::rotate( double angle )
{
  for( TVector<2> &vertex: m_vertices )
    vertex =  TMatrix<2,2>::rotationMatrix2D( angle ) * vertex;
  return *this;
}

void CComplexObject::addVertex( const TVector<2> &point )
{
  m_vertices.push_back( point - m_position );
}

double CComplexObject::rayTrace( const TVector<2> &position, const TVector<2> &direction ) const
{
  if( CObject::rayTrace( position, direction ) == HUGE_VAL )
    return HUGE_VAL;
  return HUGE_VAL;
}

void CComplexObject::spawn( double density )
{
  TVector<2> massCentreOffset = calculateCentreOfMass( m_vertices );
  m_position += massCentreOffset;
  for( auto &vertex: m_vertices )
    vertex -= massCentreOffset;

  if( isnan( density ) )
    return;

  m_attributes = TPhysicsAttributes::complexObjectAttributes( m_width, density,
                                                              m_vertices, {} );
}
