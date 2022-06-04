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

void CPhysicsEngine::step( vector<CObject*> &objects, double dt )
{
  accumulateForces( objects );
  applyForces( objects, dt );

  vector<TManifold> collisions = findCollisions( objects );

  if( m_collisionCallback )
    m_collisionCallback( collisions );

  applyImpulses( collisions );

  for( size_t i = 0; i < 0 /* todo 2 */; ++i )
  {
    resolveCollisions( collisions );
    collisions = findCollisions( objects );
  }
  resolveCollisions( collisions );


//  size_t maxIterations = 5;
//  vector<TManifold> collisions;
//
//  do
//  {
//     collisions = findCollisions( objects );
//
//    if( collisionCallback )
//      collisionCallback( collisions );
//
//    resolveCollisions( collisions );
//  } while( !collisions.empty() && --maxIterations );
//
//  for( auto &item: objects )
//  {
//    auto &physItem = dynamic_cast<CPhysicsObject &>( *item );
//    if( physItem.m_attributes.mass == INFINITY )
//      continue;
//    physItem.m_attributes.forceAccumulator = { 0, 0 };
//    for( const auto &field: m_fields )
//      field.applyForce( physItem );
//    item->m_position += physItem.m_attributes.forceAccumulator / physItem.m_attributes.mass;
//  }
//  updateObjectsOrder( objects );
//  vector<pair<CObject *, CObject *>> overlappingBBx = getPossibleCollisions( objects );
//  calculateDepths( overlappingBBx );
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
    applyImpulse( manifold );
}

void CPhysicsEngine::applyImpulse( const TManifold &manifold )
{
  for( const auto &contactPoint: manifold.contacts )
    applyImpulse( manifold, contactPoint );
}

void CPhysicsEngine::applyImpulse( const TManifold &manifold, const TContactPoint &contactPoint )
{
  TVector<2> normalImpulse = getNormalImpulse( manifold, contactPoint );

  manifold.first->applyImpulse( normalImpulse, contactPoint.contactPoint );
  manifold.second->applyImpulse( -normalImpulse, contactPoint.contactPoint );

  TVector<2> frictionImpulse = getFrictionImpulse( manifold, contactPoint );
  double frictionCoefficientSq = manifold.first->m_attributes.frictionCoefficient
          * manifold.second->m_attributes.frictionCoefficient;

  if( frictionImpulse.squareNorm() > frictionCoefficientSq * normalImpulse.squareNorm() )
    frictionImpulse.stretchTo( sqrt( frictionCoefficientSq ) * normalImpulse.norm() );

  manifold.first->applyImpulse( frictionImpulse, contactPoint.contactPoint );
  manifold.second->applyImpulse( -frictionImpulse, contactPoint.contactPoint );
}

TVector<2> CPhysicsEngine::getNormalImpulse( const TManifold &manifold, const TContactPoint &contactPoint )
{
  const auto &first = *manifold.first;
  const auto &second = *manifold.second;
  const auto &firstAttr = first.m_attributes;
  const auto &secondAttr = second.m_attributes;

  TVector<2> collisionNormal = contactPoint.overlapVector.normalized();

  const auto &collisionPoint = contactPoint.contactPoint;

  TVector<2> relativeVelocity = getRelativeVelocity( first, second,
                                                     collisionPoint );

  double elasticity = firstAttr.elasticity * secondAttr.elasticity;
  double velocityProjection = collisionNormal.dot( relativeVelocity );
  if( velocityProjection >= 0 )
    return {};

  return ( 1 + elasticity ) * velocityProjection *
         getCombinedInvMass( first, second, collisionPoint, collisionNormal ) *
         collisionNormal;
}

TVector<2> CPhysicsEngine::getFrictionImpulse( const TManifold &manifold, const TContactPoint &contactPoint )
{
  const auto &first = *manifold.first;
  const auto &second = *manifold.second;
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



void CPhysicsEngine::resolveCollisions( vector<TManifold> & )
{
  // todo
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



