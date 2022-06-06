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

void CObject::addTag( ETag tag )
{
  tags |= tag;
}

double CObject::rayTrace( const TVector<2> &position, const TVector<2> &direction ) const
{
  if( tags == WIN_ZONE )
    return HUGE_VAL;
  return NAN;
}

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

  if( m_attributes.invMass != 0 )
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


//TVector<2> lineLineCollision( const TVector<2> &aBegin,
//                              const TVector<2> &aEnd,
//                              const TVector<2> &bBegin,
//                              const TVector<2> &bEnd )
//{
//  TMatrix<2,2> overlapEquationMatrix{ aEnd - aBegin,
//                                      bBegin - bEnd };
//  if( !overlapEquationMatrix.invert() )
//    return { NAN, NAN };
//  TVector<2> solution = overlapEquationMatrix * ( bBegin - aBegin );
//  if( solution[ 0 ] <= 0 || solution[ 0 ] >= 1 ||
//      solution[ 1 ] <= 0 || solution[ 1 ] >= 1 )
//    return { NAN, NAN };
//  return aBegin + solution[ 0 ] * ( aEnd - aBegin );
//}

//TVector<2> lineCircleCollision( const TVector<2> &begin, const TVector<2> &end,
//                                const TVector<2> &centre, double radius )
//{
//  TVector<2> lineVector = end - begin;
//  double projectionScale = lineVector.dot( centre - begin )
//                           / lineVector.squareNorm();
//
//  // closest point to line that lies in line segment
//  if( projectionScale > 0 && projectionScale < 1 )
//  {
//    TVector<2> normal = ( centre - begin ).rejectedFrom( lineVector );
//    if( normal.squareNorm() > radius * radius )
//      return { NAN, NAN };
//    TVector<2> overlap = normal.stretchedTo( radius ) - normal;
//    return begin + lineVector * projectionScale + overlap / 2;
//  }
//
//  TVector<2> relativeBegin = begin - centre;
//  if( relativeBegin.squareNorm() < radius * radius )
//    return begin;
//
//  TVector<2> relativeEnd = end - centre;
//  if( relativeEnd.squareNorm() < radius * radius )
//    return end;
//
//  return { NAN, NAN };
//}

TContactPoint circleCircleCollision( const TVector<2> &firstCentre, double firstRadius,
                                     const TVector<2> &secondCentre, double secondRadius )
{
  TVector<2> relativePos = secondCentre - firstCentre;
  double relativeDist = relativePos.norm();
  double overlapSize = firstRadius + secondRadius - relativeDist;
  if( overlapSize <= 0 )
    return { {}, { NAN, NAN } };
  TVector<2> overlap = relativePos.stretchedTo( overlapSize / 2 );
  return { overlap,
           firstCentre + relativePos.stretchedTo( firstRadius ) - overlap };
}

TContactPoint rectCircleCollision( const TVector<2> &position,
                                   const TVector<2> &sizes,
                                   double rotation,
                                   const TVector<2> &centre,
                                   double radius )
{
  TMatrix<2, 4> corners = rectCorners( position, sizes, rotation );
  double smallestDist = HUGE_VAL;
  TVector<2> closestPoint;
  for( size_t idx = 0; idx < 4; ++idx )
  {
    TVector<2> temp = lineSegmentClosestPoint( corners[ idx ],
                                               corners[ ( idx + 1 ) % 4 ],
                                               centre );
    double newDist = temp.squareDistance( centre );
    if( newDist >= smallestDist )
      continue;
    smallestDist = newDist;
    closestPoint = temp;
  }

  double distance = closestPoint.distance( centre );
  if( distance >= radius )
    return { {}, { NAN, NAN } };

  TVector<2> overlap = ( centre - closestPoint );
  overlap.stretchTo( ( radius - distance ) / 2 );
  return { overlap, closestPoint - overlap };


}

TContactPoint rectRectCollision( const TVector<2> &firstPos,
                                 const TVector<2> &firstSize,
                                 double firstRot,
                                 const TVector<2> &secondPos,
                                 const TVector<2> &secondSize,
                                 double secondRot )
{
  // vector needed to move first
  TContactPoint firstSecond = firstRectOverlap( firstPos, firstSize, firstRot,
                                                secondPos, secondSize, secondRot );
  // vector needed to move second
  TContactPoint secondFirst = firstRectOverlap( secondPos, secondSize, secondRot,
                                                firstPos, firstSize, firstRot );
  if( !firstSecond.contactPoint )
    return firstSecond;
  if( !secondFirst.contactPoint )
    return secondFirst;

  if( firstSecond.overlapVector.squareNorm() >=
      secondFirst.overlapVector.squareNorm() )
    return secondFirst;

  firstSecond.overlapVector *= -1;
  return firstSecond;
}

TContactPoint firstRectOverlap( const TVector<2> &firstPos,
                                const TVector<2> &firstSize,
                                double firstRot,
                                const TVector<2> &secondPos,
                                const TVector<2> &secondSize,
                                double secondRot )
{
  TVector<2> axes[ 4 ] = {
          TVector<2>::canonical( 0 ).rotated( firstRot ),
          TVector<2>::canonical( 0 ).rotated( firstRot + M_PI_2 )
  };
  axes[ 2 ] = -axes[ 0 ];
  axes[ 3 ] = -axes[ 1 ];
  TMatrix<2, 4> firstCorners = rectCorners( firstPos, firstSize, firstRot );
  TMatrix<2, 4> secondCorners = rectCorners( secondPos, secondSize, secondRot );

  double smallestOverlap = HUGE_VAL;
  TContactPoint res;
  for( size_t idx = 0; idx < 4; ++idx )
  {
    TContactPoint temp = axisPointsPenetration( axes[ idx ],
                                                firstCorners[ ( idx + 2 ) % 4 ],
                                                secondCorners );
    if( !temp.overlapVector )
      return { {}, { NAN, NAN } };
    double dist = temp.overlapVector.squareNorm();
    if( dist < smallestOverlap )
    {
      smallestOverlap = dist;
      res = temp;
    }
  }

  return res;
}
TMatrix<2, 4> rectCorners( const TVector<2> &position,
                           const TVector<2> &size,
                           double rotation )
{
  TVector<2> firstDiagonal = size.rotated( rotation );
  TVector<2> secondDiagonal = size;
  secondDiagonal[ 1 ] *= -1;
  secondDiagonal.rotate( rotation );
  return { position + firstDiagonal, position + secondDiagonal,
           position - firstDiagonal, position - secondDiagonal };
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
    return { {}, { NAN, NAN } };

  return { normal.stretchedTo( maxOverlap / 2 ), maxPoint };
}

TVector<2> lineSegmentClosestPoint( const TVector<2> &begin,
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

TContactPoint biggestOverlap( CObject *target, const std::vector<CObject *> &objects )
{
  TContactPoint res = { {}, { NAN, NAN } };
  double norm = 0;
  for( const auto &object: objects)
  {
    TManifold collision = target->getManifold( object );
    for( const auto &contact: collision.contacts )
    {
      if( norm >= contact.overlapVector.norm() )
        continue;
      norm = contact.overlapVector.norm();
      res = contact;
    }
  }
  return res;
}

double rayTraceLineSeg( const TVector<2> &position,
                        const TVector<2> &direction,
                        const TVector<2> &begin,
                        const TVector<2> &end )
{
  TVector<2> dir = direction.normalized();
  TMatrix<2, 2> mat = { dir, begin - end };
  if( !mat.invert() )
    return HUGE_VAL;
  TVector<2> solution = mat * ( begin - position );
  if( solution[ 1 ] < 0 || solution[ 1 ] > 1 )
    return HUGE_VAL;
  return solution[ 0 ];
}