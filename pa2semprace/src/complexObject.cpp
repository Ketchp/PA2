#include "complexObject.hpp"
#include "rectangle.hpp"
#include "circle.hpp"


using namespace std;
using namespace collision;

CComplexObject::CComplexObject( double width )
        : CPhysicsObject( {}, { HUGE_VAL, HUGE_VAL } ),
          m_width( width ){}

void CComplexObject::render( CWindow &win ) const
{
  if( m_vertices.empty() )
    return;
  win.drawCircle( m_position + m_vertices[ 0 ], m_width, NAN, m_tag );
  for( size_t idx = 1; idx < m_vertices.size(); ++idx )
  {
    win.drawLine( m_position + m_vertices[ idx - 1 ], m_position + m_vertices[ idx ], m_width );
    win.drawCircle( m_position + m_vertices[ idx ], m_width, NAN, m_tag );
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

TManifold CComplexObject::getManifold( CPhysicsObject *other )
{
  return other->getManifold( this );
}

TManifold CComplexObject::getManifold( CRectangle *rect )
{
  if( m_vertices.empty() )
    return { nullptr, nullptr };

  vector<TContactPoint> contacts;
  vector<size_t> collidingFaces;

  for( size_t idx = 1; idx < m_vertices.size(); ++idx )
  {
    const auto &begin = m_vertices[ idx - 1 ];
    const auto &end = m_vertices[ idx ];
    const auto direction = end - begin;
    TContactPoint contact = rectRect( rect->m_position,
                                      rect->m_size,
                                      rect->m_rotation,
                                      m_position + ( begin + end ) / 2,
                                      { direction.norm() / 2, m_width },
                                      direction.getAngle() );
    if( !contact.contactPoint )
      continue;
    contacts.push_back( contact );
    collidingFaces.push_back( idx - 1 );
  }

  auto faceIt = collidingFaces.begin();
  for( size_t idx = 0; idx < m_vertices.size(); ++idx )
  {
    if( faceIt != collidingFaces.end() && ( *faceIt == idx || *faceIt == idx ) )
    {
      idx = *faceIt + 1;
      ++faceIt;
      continue;
    }
    TContactPoint contact = rectCircle( rect->m_position,
                                        rect->m_size,
                                        rect->m_rotation,
                                        m_vertices[ idx ] + m_position, m_width );
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
  vector<size_t> collidingFaces;
  for( size_t idx = 1; idx < m_vertices.size(); ++idx )
  {
    const auto &begin = m_vertices[ idx - 1 ];
    const auto &end = m_vertices[ idx ];
    const auto direction = end - begin;
    TContactPoint contact = rectCircle( m_position + ( begin + end ) / 2,
                                        { direction.norm() / 2, m_width },
                                        direction.getAngle(),
                                        circle->m_position,
                                        circle->m_radius );
    if( !contact.contactPoint )
      continue;
    contact.overlapVector *= -1;
    contacts.push_back( contact );
    collidingFaces.push_back( idx - 1 );
  }

  auto faceIt = collidingFaces.begin();
  for( size_t idx = 0; idx < m_vertices.size(); ++idx )
  {
    if( faceIt != collidingFaces.end() && ( *faceIt == idx || *faceIt == idx ) )
    {
      idx = *faceIt + 1;
      ++faceIt;
      continue;
    }
    TContactPoint contact = circleCircle( circle->m_position,
                                          circle->m_radius,
                                          m_vertices[ idx ] + m_position,
                                          m_width );
    if( !contact.contactPoint )
      continue;
    contacts.push_back( contact );
  }

  if( contacts.empty() )
    return { nullptr, nullptr };
  return { circle, this, contacts };
}

TManifold CComplexObject::getManifold( CComplexObject *other )
{
  if( m_vertices.empty() || other->m_vertices.empty() )
    return { nullptr, nullptr };

  vector<TContactPoint> contacts;

  for( const auto &vertex: m_vertices )
  {
    auto newContacts = getNodeCollisions( other, m_position + vertex );
    contacts.insert( contacts.end(), newContacts.begin(), newContacts.end() );
  }

  for( const auto &vertex: other->m_vertices )
  {
    auto newContacts = other->getNodeCollisions( this, other->m_position + vertex );
    for_each( newContacts.begin(), newContacts.end(),
              []( auto &elem ){ elem.overlapVector *= -1; } );
    contacts.insert( contacts.end(), newContacts.begin(), newContacts.end() );
  }


  if( contacts.empty() )
    return { nullptr, nullptr };
  return { other, this, contacts };
}

std::vector<TContactPoint> CComplexObject::getNodeCollisions( CComplexObject *other,
                                                              const TVector<2> &node ) const
{
  vector<TContactPoint> contacts;
  for( size_t faceIdx = 1; faceIdx < other->m_vertices.size(); ++faceIdx )
  {
    const auto &begin = other->m_vertices[ faceIdx - 1 ];
    const auto &end = other->m_vertices[ faceIdx ];
    const auto direction = end - begin;
    TContactPoint contact = rectCircle( other->m_position + ( begin + end ) / 2,
                                        { direction.norm() / 2, other->m_width },
                                        direction.getAngle(),
                                        node, m_width );
    if( !contact.contactPoint )
      continue;
    contacts.push_back( contact );
  }
  return contacts;
}

CPhysicsObject &CComplexObject::rotate( double angle )
{
  auto rot = TMatrix<2, 2>::rotationMatrix2D( angle );
  for( TVector<2> &vertex: m_vertices )
    vertex = rot * vertex;
  return *this;
}

void CComplexObject::addVertex( const TVector<2> &point )
{
  TVector<2> newPoint = point - m_position;
  if( m_vertices.size() )
  {
    double length = newPoint.distance( m_vertices.back() );
    if( length > m_longest )
      m_longest = length;
  }
  m_vertices.push_back( newPoint );
}

double CComplexObject::rayTrace( const TVector<2> &position, const TVector<2> &direction ) const
{
  if( CPhysicsObject::rayTrace( position, direction ) == HUGE_VAL )
    return HUGE_VAL;
  if( m_vertices.empty() )
    return HUGE_VAL;
  double smallest = HUGE_VAL;
  for( auto it = m_vertices.begin() + 1; it != m_vertices.end(); ++it )
  {
    TVector<2> normal = crossProduct( *it - *( it - 1 ) ).stretchedTo( m_width );
    TVector<2> back = m_position + *it;
    TVector<2> front = m_position + *( it - 1 );
    smallest = min( smallest, CRectangle::rayTrace( position, direction,
                                                    { back + normal,
                                                      back - normal,
                                                      front - normal,
                                                      front + normal } ) );
  }
  for( auto centre: { m_vertices.front(),
                      m_vertices.back() } )
  {
    double rayLen = CCircle::rayTrace( position, direction,
                                       m_position + centre, m_width );
    if( rayLen < 0 )
      continue;
    smallest = min( smallest, rayLen );
  }
  return smallest;
}

void CComplexObject::spawn( double density )
{
  TVector<2> massCentreOffset = calculateCentreOfMass( m_vertices );
  m_position += massCentreOffset;
  m_boundingRadius = 0;
  for( auto &vertex: m_vertices )
  {
    vertex -= massCentreOffset;
    if( vertex.norm() + m_width > m_boundingRadius )
      m_boundingRadius = vertex.norm() + m_width;
  }

  m_longest = sqrt( 0.25 * m_longest * m_longest + m_width * m_width );

  if( isnan( density ) )
  {
    return;
  }

  m_attributes = TPhysicsAttributes::complexObjectAttributes( m_width, density,
                                                              m_vertices );
}

