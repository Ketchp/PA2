#include "object.hpp"

using namespace std;

CObject::CObject( int id, TVector<2> position )
  : m_id( id ), m_position( position )
{}

CObject &CObject::rotate( double )
{
  return *this;
}

void CObject::accumulateForce( const CForceField & )
{}

void CObject::applyForce( double )
{}

void CObject::resetAccumulator()
{}

CPhysicsObject::CPhysicsObject( int id, TVector<2> position, const TPhysicsAttributes &attributes )
  : CObject( id, position ),
    m_attributes( attributes )
{}

void CPhysicsObject::accumulateForce( const CForceField &field )
{
  if( m_attributes.invMass != 0 )
    field.applyForce( *this );
}

void CPhysicsObject::applyForce( double dt )
{
  if( m_attributes.invMass == 0 )
    return;
  m_attributes.velocity += m_attributes.invMass * m_attributes.forceAccumulator * dt;
  m_attributes.angularVelocity += m_attributes.invAngularMass * m_attributes.momentAccumulator * dt;

  m_position += m_attributes.velocity * dt;
  rotate( m_attributes.angularVelocity * dt );
}

void CPhysicsObject::resetAccumulator()
{
  m_attributes.forceAccumulator = {};
  m_attributes.momentAccumulator = 0;
}

void CPhysicsObject::applyImpulse( const TVector<2> &impulse, const TVector<2> &point )
{
  m_attributes.velocity += m_attributes.invMass * impulse;

  TVector<2> lever = point - m_position;

  m_attributes.angularVelocity += lever.dot( crossProduct( impulse ) ) *
                                  m_attributes.invAngularMass;

  if( dynamic_cast<CLine *>( this ) )
  {
    cout << impulse << " at " << point << endl;
    cout << "dv = " << m_attributes.invMass * impulse << endl;
    cout << "dw = " << lever.dot( crossProduct( impulse ) ) *
                       m_attributes.invAngularMass << endl;
  }
}

TVector<2> CPhysicsObject::getLocalVelocity( const TVector<2> &point ) const
{
  TVector<2> relativePosition = point - m_position;
  return m_attributes.velocity -
  m_attributes.angularVelocity * crossProduct( relativePosition );
}

CLine::CLine( int id, TVector<2> pointStart, TVector<2> pointEnd, double density )
  : CPhysicsObject( id, ( pointStart + pointEnd ) / 2,
                    TPhysicsAttributes::lineAttributes( density,
                                                        ( pointEnd - pointStart ) / 2 ) ),
    m_direction( ( pointEnd - pointStart ) / 2 )
{}

TVector<2> CLine::begin() const
{
  return m_position - m_direction;
}

TVector<2> CLine::end() const
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

TManifold CLine::getManifold( CObject *other )
{
  return other->getManifold( this );
}

TManifold CLine::getManifold( CLine *other )
{
  TVector<2> collisionPoint = calculateCollisionPoint( begin(), end(),
                                                         other->begin(), other->end() );
  if( !collisionPoint )
    return { nullptr, nullptr };


  TVector<2> aBeginOverlap = ( collisionPoint - begin() ).rejectFrom( other->m_direction );
  TVector<2> aEndOverlap = ( collisionPoint - end() ).rejectFrom( other->m_direction );
  TVector<2> aSmallerOverlap = aBeginOverlap;
  TVector<2> aEndPointOfOverlap = begin();
  if( aBeginOverlap.norm() > aEndOverlap.norm() )
  {
    aSmallerOverlap = aEndOverlap;
    aEndPointOfOverlap = end();
  }

  TVector<2> bBeginOverlap = ( collisionPoint - other->begin() ).rejectFrom( m_direction );
  TVector<2> bEndOverlap = ( collisionPoint - other->end() ).rejectFrom( m_direction );
  TVector<2> bSmallerOverlap = bBeginOverlap;
  TVector<2> bEndPointOfOverlap = other->begin();
  if( bBeginOverlap.norm() > bEndOverlap.norm() )
  {
    bSmallerOverlap = bEndOverlap;
    bEndPointOfOverlap = other->end();
  }

  static const double overlapThreshold = 1;

  if( aSmallerOverlap.squareNorm() < bSmallerOverlap.squareNorm() &&
      overlapThreshold < aSmallerOverlap.squareNorm() )
    return { other, this, aSmallerOverlap, aEndPointOfOverlap + aSmallerOverlap / 2 };

  if( bSmallerOverlap.squareNorm() < overlapThreshold )
    return { nullptr, nullptr };

  return { this, other, bSmallerOverlap, bEndPointOfOverlap + bSmallerOverlap / 2 };
}

TManifold CLine::getManifold( CCircle *other )
{
  return other->getManifold( this );
}

TManifold CLine::getManifold( CComplexObject *other )
{
  return other->getManifold( this );
}

TVector<2> CLine::calculateCollisionPoint( TVector<2> aBegin, TVector<2> aEnd,
                                             TVector<2> bBegin, TVector<2> bEnd )
{
  TMatrix<2,2> overlapEquationMatrix{ aEnd - aBegin,
                                      bBegin - bEnd };
  if( !overlapEquationMatrix.invert() )
    return { NAN, NAN };
  TVector<2> solution = overlapEquationMatrix * ( bBegin - aBegin );
  if( solution[ 0 ] <= 0 || solution[ 0 ] >= 1 ||
      solution[ 1 ] <= 0 || solution[ 1 ] >= 1 )
    return { NAN, NAN };
  return aBegin + solution[ 0 ] * ( aEnd - aBegin );
}

//bool CLine::areColliding( const TVector<2> &aBegin, const TVector<2> &aEnd,
//                          const TVector<2> &bBegin, const TVector<2> &bEnd )
//{
//  TMatrix<2,2> overlapEquationMatrix{ aEnd - aBegin,
//                                      bBegin - bEnd };
//  if( !overlapEquationMatrix.invert() )
//    return false;
//  TVector<2> solution = overlapEquationMatrix * ( bBegin - aBegin );
//  return 0 < solution[ 0 ] && solution[ 0 ] < 1 &&
//         0 < solution[ 1 ] && solution[ 1 ] < 1;
//}

CObject &CLine::rotate( double angle )
{
  m_direction = TMatrix<2,2>::rotationMatrix2D( angle ) * m_direction;
  return *this;
}

GLUquadric *CCircle::gluRenderer = nullptr;
CCircle::CCircle( int id, TVector<2> centre, double size, double density )
  : CPhysicsObject( id, centre,
                    TPhysicsAttributes::circleAttributes( density, size ) ),
    m_size( size )
{}

void CCircle::render() const
{
  static const int circleSmoothness = 20;
  glTranslated( m_position[ 0 ], m_position[ 1 ], 0 );

  GLdouble startColor[ 4 ];
  glGetDoublev( GL_CURRENT_COLOR, startColor );


  gluDisk( gluRenderer, 0, m_size, circleSmoothness, 10 );

  TVector<2> orientation = m_size * TVector<2>::canonical( 0 ).rotated( m_rotation );

  glBegin( GL_LINES );
  glColor4d( 0.5, 0.5, 0.5, 0.5 );

  glVertex3d( 0, 0, 1 );
  glVertex3d( orientation[ 0 ], orientation[ 1 ], 1 );

  glColor4dv( startColor );
  glEnd();

  glTranslated( -m_position[ 0 ], -m_position[ 1 ], 0 );

}

TManifold CCircle::getManifold( CObject *other )
{
  return other->getManifold( this );
}

TManifold CCircle::getManifold( CLine *line )
{
  double projectionScale = line->m_direction.dot( m_position - line->m_position )
          / line->m_direction.squareNorm();

  // closest point to line that lies in line segment
  if( abs( projectionScale ) < 1 )
  {
    TVector<2> normal = m_position - line->m_position;
    normal.rejectFrom( line->m_direction );
    if( normal.squareNorm() > m_size * m_size )
      return { nullptr, nullptr };
    TVector<2> overlap = normal.stretchedTo( m_size ) - normal;
    return { line, this,
             overlap,
             line->m_position + line->m_direction * projectionScale + overlap / 2 };
  }

  TVector<2> relativeBegin = line->begin() - m_position;
  if( relativeBegin.squareNorm() < m_size * m_size )
  {
    TVector<2> overlapVector = relativeBegin.stretchedTo( m_size ) - relativeBegin;
    return { this, line,
             overlapVector,
             line->begin() + overlapVector / 2 };
  }

  TVector<2> relativeEnd = line->end() - m_position;
  if( relativeEnd.squareNorm() < m_size * m_size )
  {
    TVector<2> overlapVector = relativeEnd.stretchedTo( m_size ) - relativeEnd;
    return { this, line,
             overlapVector,
             line->end() + overlapVector / 2 };
  }

  return { nullptr, nullptr };
}

TManifold CCircle::getManifold( CCircle *other )
{
  TVector<2> relativePos = other->m_position - m_position;
  double relativeDist = relativePos.norm();
  double overlapSize = m_size + other->m_size - relativeDist;
  TVector<2> overlap = relativePos.stretchedTo( overlapSize );
  if( overlapSize <= 0 )
    return { nullptr, nullptr };
  return { this, other,
           overlap,
           m_position + relativePos.stretchedTo( m_size ) - overlap / 2
  };
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


CComplexObject::CComplexObject( int id, vector<TVector<2>> vertices, double density )
  : CPhysicsObject( id, calculateCentreOfMass( vertices ),
                    TPhysicsAttributes::complexObjectAttributes( density, vertices,
                                                                 calculateCentreOfMass( vertices ) ) ),
    m_vertices( move( vertices ) )
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

CComplexObject *CComplexObject::makeRectangle( int id, TVector<2> centre, TVector<2> size,
                                              double density )
{
  return new CComplexObject( id, vector<TVector<2>>( {
    TVector<2>{ centre[ 0 ] - size[ 0 ] / 2, centre[ 1 ] - size[ 1 ] / 2 },
    TVector<2>{ centre[ 0 ] + size[ 0 ] / 2, centre[ 1 ] - size[ 1 ] / 2 },
    TVector<2>{ centre[ 0 ] + size[ 0 ] / 2, centre[ 1 ] + size[ 1 ] / 2 },
    TVector<2>{ centre[ 0 ] - size[ 0 ] / 2, centre[ 1 ] + size[ 1 ] / 2 },
    TVector<2>{ centre[ 0 ] - size[ 0 ] / 2, centre[ 1 ] - size[ 1 ] / 2 } } ), density );
}

TVector<2> CComplexObject::calculateCentreOfMass( const vector<TVector<2>> &vertices )
{
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

TManifold CComplexObject::getManifold( CLine *other )
{
  return { this, other, { 0, 0 }, { 0, 0 } }; // todo
}

TManifold CComplexObject::getManifold( CCircle *other )
{
  return { this, other, { 0, 0 }, { 0, 0 } }; // todo
}

TManifold CComplexObject::getManifold( CComplexObject *other )
{
  return { this, other, { 0, 0 }, { 0, 0 } }; // todo
}

CObject &CComplexObject::rotate( double angle )
{
  for( TVector<2> &vertex: m_vertices )
    vertex =  TMatrix<2,2>::rotationMatrix2D( angle ) * vertex;
  return *this;
}

