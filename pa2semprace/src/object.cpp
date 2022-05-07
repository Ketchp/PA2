#include "object.hpp"

CObject::CObject( math::vector position, double rotation )
  : m_position( position ), m_rotation( rotation )
{}

CPhysicsObject::CPhysicsObject( math::vector position, double rotation, const physicsAttributes &attributes )
  : CObject( position, rotation ),
    m_attributes( attributes )
{}

CPhysicsObject::CPhysicsObject( math::vector position, const physicsAttributes &attributes )
  : CObject( position, 0 ),
    m_attributes( attributes )
{}

CLine::CLine( math::vector pointStart, math::vector pointEnd, const physicsAttributes &attributes )
  : CPhysicsObject( ( pointStart + pointEnd ) / 2, attributes ),
    m_direction( ( pointEnd - pointStart ) / 2 )
{}

math::vector CLine::begin() const
{
  return m_position - m_direction.rotated( m_rotation );
}

math::vector CLine::end() const
{
  return m_position + m_direction.rotated( m_rotation );
}

void CLine::render() const
{
  glBegin( GL_LINES );

  glVertex2d( begin()[ 0 ], begin()[ 1 ] );
  glVertex2d( end()[ 0 ], end()[ 1 ] );

  glEnd();
}

void CLine::updateBoundingBox()
{
  m_BBx = { std::min( begin()[ 0 ], end()[ 0 ] ),
            std::max( begin()[ 0 ], end()[ 0 ] ) };
//  m_BBy = { std::min( begin()[ 1 ], end()[ 1 ] ),
//            std::max( begin()[ 1 ], end()[ 1 ] ) };
}

CManifold CLine::getManifold( CObject *other )
{
  auto *otherLine = dynamic_cast<CLine *>( other );
  if( otherLine )
    return otherLine->getManifold( this );
  auto *otherCircle = dynamic_cast<CCircle *>( other );
  if( otherCircle )
    return otherCircle->getManifold( this );
  auto *otherComplex = dynamic_cast<CComplexObject *>( other );
  if( otherComplex )
    return otherComplex->getManifold( this );
  throw std::invalid_argument( "No collision detection for this object." );
}

CManifold CLine::getManifold( CLine *other )
{
  math::matrix overlapEquationMatrix{ begin() - end(),
                                      other->end() - other->begin() };
}

GLUquadric *CCircle::gluRenderer = nullptr;
CCircle::CCircle( math::vector centre, double size, const physicsAttributes &attributes )
  : CPhysicsObject( centre, attributes ),
    m_size( size )
{}

void CCircle::render() const
{
  static const int circleSmoothness = 20;

  glTranslated( m_position[ 0 ], m_position[ 1 ], 0 );

  gluDisk( gluRenderer, 0, m_size, circleSmoothness, 10 );

  glTranslated( -m_position[ 0 ], -m_position[ 1 ], 0 );
}

void CCircle::updateBoundingBox()
{
  m_BBx = { m_position[ 0 ] - m_size, m_position[ 0 ] + m_size };
//  m_BBy = { m_position[ 1 ] - m_size, m_position[ 1 ] + m_size };
}


CComplexObject::CComplexObject( std::vector<math::vector> vertices, const physicsAttributes &attributes )
  : CPhysicsObject( calculateCentreOfMass( vertices ), attributes ),
    m_vertices( std::move( vertices ) )
{
  for( auto &vertex: m_vertices )
    vertex -= m_position;
}

void CComplexObject::render() const
{
  glBegin( GL_LINE_STRIP );
  for( const auto &vertex: m_vertices )
    glVertex2d( vertex[ 0 ] + m_position[ 0 ], vertex[ 1 ] + m_position[ 1 ] );
  glEnd();
}

CComplexObject CComplexObject::makeRectangle( math::vector centre, math::vector size,
                                              const physicsAttributes &attributes )
{
  return CComplexObject( std::vector<math::vector>( {
    math::vector{ centre[ 0 ] - size[ 0 ] / 2, centre[ 1 ] - size[ 1 ] / 2 },
    math::vector{ centre[ 0 ] + size[ 0 ] / 2, centre[ 1 ] - size[ 1 ] / 2 },
    math::vector{ centre[ 0 ] + size[ 0 ] / 2, centre[ 1 ] + size[ 1 ] / 2 },
    math::vector{ centre[ 0 ] - size[ 0 ] / 2, centre[ 1 ] + size[ 1 ] / 2 },
    math::vector{ centre[ 0 ] - size[ 0 ] / 2, centre[ 1 ] - size[ 1 ] / 2 } } ), attributes );
}

void CComplexObject::updateBoundingBox()
{
  double minX = HUGE_VAL,
         minY = HUGE_VAL,
         maxX = -HUGE_VAL,
         maxY = -HUGE_VAL;
  for( const auto &vertex: m_vertices  )
  {
    math::vector rotated = vertex.rotated( m_rotation );
    math::vector plus = m_position + rotated;
    math::vector minus = m_position - rotated;
    minX = std::min( std::min( plus[ 0 ], minus[ 0 ] ), minX );
    maxX = std::max( std::max( plus[ 0 ], minus[ 0 ] ), maxX );
    minY = std::min( std::min( plus[ 1 ], minus[ 1 ] ), minY );
    maxY = std::max( std::max( plus[ 1 ], minus[ 1 ] ), maxY );
  }
}

math::vector CComplexObject::calculateCentreOfMass( const std::vector<math::vector> &vertices )
{
  double weight = 0;
  math::vector weightedPosition;
  for( size_t idx = 1; idx < vertices.size(); ++idx )
  {
    math::vector direction = vertices[ idx ] - vertices[ idx - 1 ];
    math::vector position = vertices[ idx ] + vertices[ idx - 1 ]; // *2
    weight += direction.norm();
    weightedPosition += direction.norm() * position;
  }
  return weightedPosition / weight / 2;
}

