#include "object.hpp"

CObject::CObject( math::vector position, double rotation )
  : m_position( position ), m_rotation( rotation )
{}

CObject &CObject::rotate( double angle )
{
  m_rotation += angle;
  m_rotation -= (int)( m_rotation / ( 2 * M_PI ) ) * ( 2 * M_PI );
  return *this;
}

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
  return m_position - m_direction;
}

math::vector CLine::end() const
{
  return m_position + m_direction;
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
  math::vector collisionPoint = calculateCollisionPoint( begin(), end(),
                                                         other->begin(), other->end() );
  if( !collisionPoint )
    return { nullptr, nullptr, { 0, 0 }, { 0, 0 } };


  math::vector aBeginOverlap = ( collisionPoint - begin() ).rejectFrom( other->m_direction );
  math::vector aEndOverlap = ( collisionPoint - end() ).rejectFrom( other->m_direction );
  math::vector aSmallestOverlap = aBeginOverlap;
  math::vector aEndPointOfOverlap = begin();
  if( aBeginOverlap.norm() > aEndOverlap.norm() )
  {
    aSmallestOverlap = aEndOverlap;
    aEndPointOfOverlap = end();
  }

  math::vector bBeginOverlap = ( collisionPoint - other->begin() ).rejectFrom( m_direction );
  math::vector bEndOverlap = ( collisionPoint - other->end() ).rejectFrom( m_direction );
  math::vector bSmallestOverlap = bBeginOverlap;
  math::vector bEndPointOfOverlap = other->begin();
  if( bBeginOverlap.norm() > bEndOverlap.norm() )
  {
    bSmallestOverlap = bEndOverlap;
    bEndPointOfOverlap = other->end();
  }

  if( aSmallestOverlap.norm() < bSmallestOverlap.norm() )
    return { other, this, aSmallestOverlap, aEndPointOfOverlap + aSmallestOverlap / 2 };
  return { this, other, bSmallestOverlap, bEndPointOfOverlap + bSmallestOverlap / 2 };
}

math::vector CLine::calculateCollisionPoint( math::vector aBegin, math::vector aEnd,
                                             math::vector bBegin, math::vector bEnd )
{
  math::matrix overlapEquationMatrix{ aEnd - aBegin,
                                      bBegin - bEnd };
  if( !overlapEquationMatrix.invert() )
    return { NAN, NAN };
  math::vector solution = overlapEquationMatrix * ( bBegin - aBegin );
  return aBegin + solution[ 0 ] * ( aEnd - aBegin );
}

bool CLine::areColliding( const math::vector &aBegin, const math::vector &aEnd,
                          const math::vector &bBegin, const math::vector &bEnd )
{
  math::matrix overlapEquationMatrix{ aEnd - aBegin,
                                      bBegin - bEnd };
  if( !overlapEquationMatrix.invert() )
    return false;
  math::vector solution = overlapEquationMatrix * ( bBegin - aBegin );
  return 0 < solution[ 0 ] && solution[ 0 ] < 1 &&
         0 < solution[ 1 ] && solution[ 1 ] < 1;
}

CObject &CLine::rotate( double angle )
{
  m_direction = math::matrix::rotationMatrix( angle ) * m_direction;
  return *this;
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

CManifold CCircle::getManifold( CObject *other )
{
  auto *otherCircle = dynamic_cast<CCircle *>( other );
  if( otherCircle )
    return otherCircle->getManifold( this );
  auto *otherComplex = dynamic_cast<CComplexObject *>( other );
  if( otherComplex )
    return otherComplex->getManifold( this );
  throw std::invalid_argument( "No collision detection for this object." );
}

CManifold CCircle::getManifold( CLine *other )
{
  double projectionScale = other->m_direction.dot( m_position - other->m_position )
          / other->m_direction.squareNorm();
  if( -1 < projectionScale && projectionScale < 1 )
  {
    math::vector normal = ( m_position - other->m_position ).rejectFrom( other->m_direction );
    if( normal.squareNorm() > m_size * m_size )
      return { nullptr, nullptr, { 0, 0 }, { 0, 0 } };
    return { other, this,
             normal,
             m_position - normal / 2 };
  }

  math::vector relativeBegin = other->begin() - m_position;
  if( relativeBegin.squareNorm() < m_size * m_size )
  {
    double distFromBegin = relativeBegin.norm();
    math::vector overlapVector = ( ( m_size - distFromBegin ) / distFromBegin ) * relativeBegin;
    return { this, other,
             overlapVector,
             other->begin() + overlapVector / 2
    };
  }

  math::vector relativeEnd = other->end() - m_position;
  if( relativeEnd.squareNorm() < m_size * m_size )
  {
    double distFromEnd = relativeEnd.norm();
    math::vector overlapVector = ( ( m_size - distFromEnd ) / distFromEnd ) * relativeEnd;
    return { this, other,
             overlapVector,
             other->end() + overlapVector / 2
    };
  }

  return { this, other,
           { 0, 0 },
           { 0, 0 } };
}

CManifold CCircle::getManifold( CCircle *other )
{
  math::vector relativePos = other->m_position - m_position;
  double relativeDist = relativePos.norm();
  if( relativeDist > m_size + other->m_size )
    return { nullptr, nullptr, { 0, 0 }, { 0, 0 } };
  return { this, other,
           ( ( m_size + other->m_size - relativeDist ) / relativeDist ) * relativePos,
           ( ( other->m_size - m_size + relativeDist ) / 2 ) * relativePos + m_position
  };
}

CObject &CCircle::rotate( double angle )
{
  return *this;
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
    minX = std::min( vertex[ 0 ], minX );
    maxX = std::max( vertex[ 0 ], maxX );
    minY = std::min( vertex[ 1 ], minY );
    maxY = std::max( vertex[ 1 ], maxY );
  }
  m_BBx = { minX, maxX };
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

CManifold CComplexObject::getManifold( CObject *other )
{
  auto *otherComplex = dynamic_cast<CComplexObject *>( other );
  if( otherComplex )
    return otherComplex->getManifold( this );
  throw std::invalid_argument( "No collision detection for this object." );
}

CManifold CComplexObject::getManifold( CLine *other )
{
  return { this, other, { 0, 0 }, { 0, 0 } }; // todo
}

CManifold CComplexObject::getManifold( CCircle *other )
{
  return { this, other, { 0, 0 }, { 0, 0 } }; // todo
}

CManifold CComplexObject::getManifold( CComplexObject *other )
{
  return { this, other, { 0, 0 }, { 0, 0 } }; // todo
}

CObject &CComplexObject::rotate( double angle )
{
  for( math::vector &vertex: m_vertices )
    vertex =  math::matrix::rotationMatrix( angle ) * vertex;
  return *this;
}

