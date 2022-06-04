#include "object.hpp"

using namespace std;

CObject::CObject( int id, TVector<2> position )
  : m_id( id ), m_position( position )
{}

void CObject::accumulateForce( const CForceField & )
{}

void CObject::applyForce( double )
{}

void CObject::resetAccumulator()
{}

CPhysicsObject::CPhysicsObject( int id, TVector<2> position,
                                const TPhysicsAttributes &attributes,
                                double angle )
  : CObject( id, position ),
    m_attributes( attributes ),
    m_rotation( angle )
{}

CObject &CPhysicsObject::rotate( double angle )
{
  m_rotation += angle;
  return *this;
}

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

  if( dynamic_cast<CRectangle *>( this ) )
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

CRectangle::CRectangle( int id, TVector<2> centrePoint,
                        double width, double height,
                        double rotation, double density )
  : CPhysicsObject( id, centrePoint,
                    TPhysicsAttributes::rectangleAttributes( density,
                                                             width,
                                                             height ),
                    rotation ),
    m_direction{ width / 2, height / 2 },
    id( id )
{}

void CRectangle::render( const CWindow &win ) const
{
  win.drawLine( left(), right(), m_direction[ 1 ] );
}

TManifold CRectangle::getManifold( CObject *other )
{
  return other->getManifold( this );
}

TManifold CRectangle::getManifold( CRectangle *other )
{

  TManifold firstSecond = rectOverlap( other );
  TManifold secondFirst = other->rectOverlap( this );
  if( !firstSecond )
    return firstSecond;
  if( !secondFirst )
    return secondFirst;
  if( firstSecond.contacts[ 0 ].overlapVector.squareNorm() <
      secondFirst.contacts[ 0 ].overlapVector.squareNorm() )
    return firstSecond;
  return secondFirst;
}

TManifold CRectangle::rectOverlap( const CRectangle *other ) const
{
  TVector<2> axes[ 4 ] = {
          TVector<2>::canonical( 0 ).rotated( m_rotation ),
          TVector<2>::canonical( 0 ).rotated( m_rotation + M_PI_2 )
  };
  axes[ 2 ] = -axes[ 0 ];
  axes[ 3 ] = -axes[ 1 ];
  TMatrix<2, 4> firstCorners = corners();
  TMatrix<2, 4> secondCorners = other->corners();

  double smallestOverlap = HUGE_VAL;
  TContactPoint res;
  for( size_t idx = 0; idx < 4; ++idx )
  {
    TContactPoint temp = axisPointsPenetration( axes[ idx ],
                                                firstCorners[ ( idx + 2 ) % 4 ],
                                                secondCorners );
    if( !temp.overlapVector )
      return { nullptr, nullptr };
    double dist = temp.overlapVector.squareNorm();
    if( dist < smallestOverlap )
    {
      smallestOverlap = dist;
      res = temp;
    }
  }

  return { (CObject *)other, (CObject *)this, res };
}

TManifold CRectangle::getManifold( CCircle *circle )
{
  // rough estimate
//  if( m_position.squareDistance( circle->m_position ) >
//      ( m_direction.squareNorm() + circle->m_radius * circle->m_radius ) )
//    return { nullptr, nullptr };

  TVector<2> closestPoint = rectangleClosestPoint( circle->m_position );
  double distance = closestPoint.distance( circle->m_position );
  if( distance >= circle->m_radius )
    return { nullptr, nullptr };

  TVector<2> overlap = ( circle->m_position - closestPoint );
  overlap.stretchTo( ( circle->m_radius - distance ) / 2 );
  return { this, circle, overlap, closestPoint - overlap };
}

TManifold CRectangle::getManifold( CComplexObject *other )
{
  return other->getManifold( this );
}

CObject &CRectangle::rotate( double angle )
{
  CPhysicsObject::rotate( angle );
  return *this;
}

TVector<2> CRectangle::left() const
{
  TVector<2> dir = m_direction;
  dir[ 1 ] = 0;
  dir.rotate( m_rotation );
  return m_position - dir;
}

TVector<2> CRectangle::right() const
{
  TVector<2> dir = m_direction;
  dir[ 1 ] = 0;
  dir.rotate( m_rotation );
  return m_position + dir;
}

TMatrix<2, 4> CRectangle::corners() const
{
  TVector<2> firstDiagonal = m_direction.rotated( m_rotation );
  TVector<2> secondDiagonal = m_direction;
  secondDiagonal[ 1 ] *= -1;
  secondDiagonal.rotate( m_rotation );
  return { m_position + firstDiagonal, m_position + secondDiagonal,
           m_position - firstDiagonal, m_position - secondDiagonal };
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

TVector<2> CRectangle::lineSegmentClosestPoint( const TVector<2> &begin,
                                                const TVector<2> &end,
                                                const TVector<2> &point )
{
  TVector<2> direction = end - begin;
  double projectionScale = direction.dot( point - begin ) / direction.squareNorm();
  if( projectionScale <= 0 )
    return begin;
  if( projectionScale >= 1 )
    return end;
  return begin + direction * projectionScale;
}

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
  TVector<2> relativePos = other->m_position - m_position;
  double relativeDist = relativePos.norm();
  double overlapSize = m_radius + other->m_radius - relativeDist;
  TVector<2> overlap = relativePos.stretchedTo( overlapSize );
  if( overlapSize <= 0 )
    return { nullptr, nullptr };
  return { this, other,
           overlap,
           m_position + relativePos.stretchedTo( m_radius ) - overlap / 2
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

CComplexObject::CComplexObject()
  : CPhysicsObject( 0, { NAN, NAN }, { HUGE_VAL, HUGE_VAL }, 0 )
{}

CComplexObject::CComplexObject( int id, vector<TVector<2>> vertices, double density )
  : CPhysicsObject( id, calculateCentreOfMass( vertices ),
                    TPhysicsAttributes::complexObjectAttributes( density, vertices,
                                                                 calculateCentreOfMass( vertices ) ),
                    0 ),
    m_vertices( move( vertices ) )
{
  for( auto &vertex: m_vertices )
    vertex -= m_position;
}

void CComplexObject::render( const CWindow &win ) const
{
  if( m_vertices.empty() )
    return;
  win.drawCircle( m_vertices[ 0 ], 10 );
  for( size_t idx = 1; idx < m_vertices.size(); ++idx )
  {
    win.drawLine( m_vertices[ idx - 1 ], m_vertices[ idx ], 10 );
    win.drawCircle( m_vertices[ idx ], 10 );
  }
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

TManifold CComplexObject::getManifold( CRectangle *other )
{
  return { nullptr, nullptr }; // todo
}

TManifold CComplexObject::getManifold( CCircle *other )
{
  return { nullptr, nullptr }; // todo
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
  m_vertices.push_back( point );
}

TVector<2> lineLineCollision( const TVector<2> &aBegin,
                              const TVector<2> &aEnd,
                              const TVector<2> &bBegin,
                              const TVector<2> &bEnd )
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

TVector<2> lineCircleCollision( const TVector<2> &begin, const TVector<2> &end,
                                const TVector<2> &centre, double radius )
{
  TVector<2> lineVector = end - begin;
  double projectionScale = lineVector.dot( centre - begin )
                           / lineVector.squareNorm();

  // closest point to line that lies in line segment
  if( projectionScale > 0 && projectionScale < 1 )
  {
    TVector<2> normal = ( centre - begin ).rejectedFrom( lineVector );
    if( normal.squareNorm() > radius * radius )
      return { NAN, NAN };
    TVector<2> overlap = normal.stretchedTo( radius ) - normal;
    return begin + lineVector * projectionScale + overlap / 2;
  }

  TVector<2> relativeBegin = begin - centre;
  if( relativeBegin.squareNorm() < radius * radius )
    return begin;

  TVector<2> relativeEnd = end - centre;
  if( relativeEnd.squareNorm() < radius * radius )
    return end;

  return { NAN, NAN };
}

TVector<2> circleCircleCollision( const TVector<2> &firstCentre, double firstRadius,
                                  const TVector<2> &secondCentre, double secondRadius )
{
  TVector<2> relativePos = secondCentre - firstCentre;
  double relativeDist = relativePos.norm();
  double overlapSize = firstRadius + secondRadius - relativeDist;
  if( overlapSize <= 0 )
    return { NAN, NAN };
  TVector<2> overlap = relativePos.stretchedTo( overlapSize );
  return firstCentre + relativePos.stretchedTo( firstRadius ) - overlap / 2;
}

double separation( const TVector<2> &axis,
                   const TMatrix<2, 4> &firstPoints,
                   const TMatrix<2, 4> &secondPoints )
{
  TVector<2> normal = crossProduct( axis );
  double firstProjectionValues[4] = {
          normal.dot( firstPoints[ 0 ] ),
          normal.dot( firstPoints[ 1 ] ),
          normal.dot( firstPoints[ 2 ] ),
          normal.dot( firstPoints[ 3 ] ),
  };
  double secondProjectionValues[4] = {
          normal.dot( secondPoints[ 0 ] ),
          normal.dot( secondPoints[ 1 ] ),
          normal.dot( secondPoints[ 2 ] ),
          normal.dot( secondPoints[ 3 ] ),
  };
  double firstMin = *min_element( firstProjectionValues,
                                  firstProjectionValues + 4 );
  double firstMax = *max_element( firstProjectionValues,
                                  firstProjectionValues + 4 );
  double secondMin = *min_element( secondProjectionValues,
                                   secondProjectionValues + 4 );
  double secondMax = *max_element( secondProjectionValues,
                                   secondProjectionValues + 4 );

  return min( secondMin - firstMax,
              secondMax - firstMin );
}

template <size_t dim>
TContactPoint axisPointsPenetration( const TVector<2> &axisDirection,
                                     const TVector<2> &axisPoint,
                                     const TMatrix<2, dim> &points )
{
  TVector<2> normal = crossProduct( axisDirection ).normalized();
  double maxOverlap = -HUGE_VAL;
  TVector<2> maxPoint;
  for( size_t idx = 0; idx < dim; ++idx )
  {
    const TVector<2> &point = points[ idx ];
    double projectionScale = normal.dot( point - axisPoint );
    if( projectionScale <= maxOverlap )
      continue;
    maxOverlap = projectionScale;
    maxPoint = point;
  }

  if( maxOverlap <= 0 )
    return { TVector<2>{ NAN, NAN },
             TVector<2>{ NAN, NAN } };

  return { normal.stretchedTo( maxOverlap / 2 ), maxPoint };
}
