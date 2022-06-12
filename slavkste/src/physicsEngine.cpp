#include "physicsEngine.hpp"
#include "object.hpp"


using namespace std;


void CPhysicsEngine::addField( CForceField field )
{
  m_fields.emplace_back( move( field ) );
}

void CPhysicsEngine::reset()
{
  frame = 0;
  m_fields.clear();
}

vector<TManifold> CPhysicsEngine::step( vector<CPhysicsObject *> &objects, double dt )
{
  accumulateForces( objects );
  applyForces( objects, dt );

  vector<TManifold> allCollisions = findCollisions( objects );

  applyImpulses( allCollisions );
  resolveCollisions( allCollisions );

  for( size_t iteration = 0; iteration < 1; ++iteration )
  {
    vector<TManifold> collisions = findCollisions( objects );
    resolveCollisions( collisions );
    allCollisions.insert( allCollisions.end(), collisions.begin(), collisions.end() );
  }
  ++frame;
  return allCollisions;
}

void CPhysicsEngine::accumulateForces( vector<CPhysicsObject *> &objects )
{
  for( auto &item: objects )
  {
    item->resetAccumulator();
    for( const auto &field: m_fields )
      field.applyForce( *item );
  }
}

void CPhysicsEngine::applyForces( vector<CPhysicsObject *> &objects, double dt )
{
  for( auto &item: objects )
    item->applyForce( dt );
}

vector<TManifold> CPhysicsEngine::findCollisions( vector<CPhysicsObject *> &objects )
{
  vector<TManifold> collisions;
  for( size_t a = 0; a < objects.size(); ++a )
    for( size_t b = a + 1; b < objects.size(); ++b )
    {
      if( objects[ a ]->m_boundingRadius + objects[ b ]->m_boundingRadius <
          ( objects[ a ]->m_position - objects[ b ]->m_position ).norm() )
        continue;
      TManifold collision = objects[ a ]->getManifold( objects[ b ] );
      if( collision )
        collisions.push_back( collision );
    }
  return collisions;
}

void CPhysicsEngine::applyImpulses( vector<TManifold> &manifolds )
{
  for( auto &manifold: manifolds )
  {
    if( manifold.first->m_tag & ETag::NON_SOLID ||
        manifold.second->m_tag & ETag::NON_SOLID )
      continue;
    applyImpulse( manifold );
  }
}

void CPhysicsEngine::applyImpulse( const TManifold &manifold )
{
  for( const auto &contactPoint: manifold.contacts )
    applyImpulse( *manifold.first, *manifold.second, contactPoint );
}

void CPhysicsEngine::applyImpulse( CPhysicsObject &first,
                                   CPhysicsObject &second,
                                   const TContactPoint &contactPoint )
{
  TVector<2> normalImpulse = getNormalImpulse( first, second, contactPoint );
  if( !normalImpulse )
    return;

  first.applyImpulse( normalImpulse, contactPoint.contactPoint );
  second.applyImpulse( -normalImpulse, contactPoint.contactPoint );

  TVector<2> frictionImpulse = getFrictionImpulse( first, second, contactPoint );
  double frictionCoefficientSq = first.m_attributes.frictionCoefficient
                                 * second.m_attributes.frictionCoefficient;

  if( frictionImpulse.squareNorm() > frictionCoefficientSq * normalImpulse.squareNorm() )
    frictionImpulse.stretchTo( sqrt( frictionCoefficientSq ) * normalImpulse.norm() );

  first.applyImpulse( frictionImpulse, contactPoint.contactPoint );
  second.applyImpulse( -frictionImpulse, contactPoint.contactPoint );
}

TVector<2> CPhysicsEngine::getNormalImpulse( CPhysicsObject &first,
                                             CPhysicsObject &second,
                                             const TContactPoint &contactPoint )
{
  const auto &firstAttr = first.m_attributes;
  const auto &secondAttr = second.m_attributes;

  TVector<2> collisionNormal = contactPoint.overlapVector.normalized();

  const auto &collisionPoint = contactPoint.contactPoint;

  TVector<2> relativeVelocity = getRelativeVelocity( first, second,
                                                     collisionPoint );

  double elasticity = firstAttr.elasticity * secondAttr.elasticity;
  double velocityProjection = collisionNormal.dot( relativeVelocity );
  if( velocityProjection >= 0 )
    return { NAN, NAN };

  return ( 1 + elasticity ) * velocityProjection *
         getCombinedInvMass( first, second, collisionPoint, collisionNormal ) *
         collisionNormal;
}

TVector<2> CPhysicsEngine::getFrictionImpulse( CPhysicsObject &first,
                                               CPhysicsObject &second,
                                               const TContactPoint &contactPoint )
{
  const auto &firstAttr = first.m_attributes;
  const auto &secondAttr = second.m_attributes;

  TVector<2> collisionTangent = crossProduct( contactPoint.overlapVector.normalized() );

  const auto &collisionPoint = contactPoint.contactPoint;

  TVector<2> relativeVelocity = getRelativeVelocity( first, second,
                                                     collisionPoint );

  double frictionCoefficient = firstAttr.frictionCoefficient * secondAttr.frictionCoefficient;

  return ( 1 + frictionCoefficient ) *
         collisionTangent.dot( relativeVelocity ) *
         getCombinedInvMass( first, second, collisionPoint, collisionTangent ) *
         collisionTangent;
}

TVector<2> CPhysicsEngine::getRelativeVelocity( const CPhysicsObject &first,
                                                const CPhysicsObject &second,
                                                const TVector<2> &pointOfContact )
{
  TVector<2> firstVelocity = first.getLocalVelocity( pointOfContact );
  TVector<2> secondVelocity = second.getLocalVelocity( pointOfContact );
  return secondVelocity - firstVelocity;
}


double CPhysicsEngine::getCombinedInvMass( const CPhysicsObject &first,
                                           const CPhysicsObject &second,
                                           const TVector<2> &point,
                                           const TVector<2> &direction )
{
  TVector<2> directionNormalized = direction.normalized();
  return 1 / ( getObjectInvMass( first, point, directionNormalized ) +
               getObjectInvMass( second, point, directionNormalized ) );
}

double CPhysicsEngine::getObjectInvMass( const CPhysicsObject &object,
                                         const TVector<2> &point,
                                         const TVector<2> &direction )
{
  double linearInvMass = object.m_attributes.invMass;
  TVector<2> lever = point - object.m_position;

  double angMass = pow( lever.dot( crossProduct( direction ) ), 2 ) * object.m_attributes.invAngularMass;

  return linearInvMass + angMass;
}

void CPhysicsEngine::resolveCollisions( std::vector<TManifold> &collisions )
{
  for( auto &collision: collisions )
  {
    if( collision.first->m_tag & ETag::NON_SOLID ||
        collision.second->m_tag & ETag::NON_SOLID )
      continue;
    resolveCollision( collision );
  }
}

void CPhysicsEngine::resolveCollision( const TManifold &collision )
{
  TVector<2> overlapVector;
  for( const auto &contactPoint: collision.contacts )
    overlapVector += contactPoint.overlapVector;
  overlapVector /= (double)collision.contacts.size() * 2;
  resolveCollision( *collision.first, *collision.second, overlapVector );
}

void CPhysicsEngine::resolveCollision( CPhysicsObject &first,
                                       CPhysicsObject &second,
                                       const TVector<2> &overlapVector )
{
  double linearInvMass = 1 / ( first.m_attributes.invMass +
                               second.m_attributes.invMass );
  if( !isnormal( linearInvMass ) )
    return;
  first.m_position -= overlapVector * linearInvMass * first.m_attributes.invMass;
  second.m_position += overlapVector * linearInvMass * second.m_attributes.invMass;
}
