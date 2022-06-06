#include "physicsEngine.hpp"
#include "object.hpp"

using namespace std;

CForceField::CForceField( function<void(CPhysicsObject &)> functor )
  : m_fieldFunctor( move(functor) )
{}

void CForceField::applyForce( CPhysicsObject &obj ) const
{
  m_fieldFunctor( obj );
}

CForceField CForceField::gravitationalField( double g )
{
  return CForceField( [g]( CPhysicsObject &object )
  {
    object.m_attributes.forceAccumulator += g * object.m_attributes.mass * TVector<2>{ 0, -1 };
  } );
}


void CPhysicsEngine::addField( CForceField field )
{
  m_fields.emplace_back( move( field ) );
}

void CPhysicsEngine::registerCollisionCallback(
        const std::function<void(std::vector<TManifold>)> &callback )
{
  m_collisionCallback = callback;
}

void CPhysicsEngine::step( vector<CObject*> &objects, double dt )
{
  accumulateForces( objects );
  applyForces( objects, dt );

  vector<TManifold> collisions = findCollisions( objects );

  if( m_collisionCallback )
    m_collisionCallback( collisions );

  applyImpulses( collisions );
  resolveCollisions( collisions );

  for( size_t iteration = 0; iteration < 1; ++iteration )
  {
    collisions = findCollisions( objects );
    resolveCollisions( collisions );
  }
}

void CPhysicsEngine::accumulateForces( vector<CObject *> &objects )
{
  for( auto &item: objects )
  {
    item->resetAccumulator();
    for( const auto &field: m_fields )
      item->accumulateForce( field );
  }
}

void CPhysicsEngine::applyForces( vector<CObject *> &objects, double dt )
{
  for( auto &item: objects )
    item->applyForce( dt );
}

vector<TManifold> CPhysicsEngine::findCollisions( vector<CObject *> &objects )
{
  vector<TManifold> collisions;
  for( auto firstIt = objects.begin(); firstIt != objects.end(); ++firstIt )
    for( auto secondIt = firstIt + 1; secondIt != objects.end(); ++secondIt )
    {
      TManifold collision = (*firstIt)->getManifold( *secondIt );
      if( collision )
        collisions.push_back( collision );
    }
  return collisions;
}

void CPhysicsEngine::applyImpulses( vector<TManifold> &manifolds )
{
  for( auto &manifold: manifolds )
  {
    if( manifold.first->tags & ETag::ZONE ||
        manifold.second->tags & ETag::ZONE )
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

  // todo limit friction

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
  TVector<3> lever = TVector<3>::changeDim( point - object.m_position );
  TVector<3> projection = TVector<3>::changeDim( direction );

  double angMass = crossProduct(
                       crossProduct(
                           lever,
                           projection ),
                       lever ).dot( projection ) * object.m_attributes.invAngularMass;

  return linearInvMass + angMass;
}

void CPhysicsEngine::resolveCollisions( std::vector<TManifold> &collisions )
{
  for( auto &collision: collisions )
  {
    if( collision.first->tags & ETag::ZONE ||
        collision.second->tags & ETag::ZONE )
      continue;
    resolveCollision( collision );
  }
}

void CPhysicsEngine::resolveCollision( const TManifold &collision )
{
  for( const auto &contactPoint: collision.contacts )
    resolveCollision( *collision.first, *collision.second, contactPoint );
}

void CPhysicsEngine::resolveCollision( CPhysicsObject &first,
                                       CPhysicsObject &second,
                                       const TContactPoint &contact )
{
//  TVector<2> relativeVelocity = getRelativeVelocity( first, second, contact.contactPoint );
//  TVector<2> collisionNormal = contact.overlapVector;
//  if( collisionNormal.dot( relativeVelocity ) >= 0 )
//    return;

  double linearInvMass = 1 / ( first.m_attributes.invMass +
                               second.m_attributes.invMass );
  if( !isnormal( linearInvMass ) )
    return;
  first.m_position -= contact.overlapVector * linearInvMass * first.m_attributes.invMass;
  second.m_position += contact.overlapVector * linearInvMass * second.m_attributes.invMass;

}


