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
    object.m_attributes.forceAccumulator += g / object.m_attributes.invMass * TVector<2>{ 0, -1 };
  } );
}

TManifold::TManifold( CObject *first, CObject *second )
  : first( dynamic_cast<CPhysicsObject *>( first ) ),
    second( dynamic_cast<CPhysicsObject *>( second ) )
{}

TManifold::TManifold( CObject *first, CObject *second,
                      TVector<2> overlapVector,
                      TVector<2> contactPoint )
  : TManifold( first, second )
{
  contacts.push_back( { overlapVector, contactPoint } );
}


void CPhysicsEngine::addField( CForceField field )
{
  m_fields.emplace_back( move( field ) );
}

void CPhysicsEngine::step( vector<CObject*> &objects )
{
  accumulateForces( objects );
  applyForces( objects );

  vector<TManifold> collisions = findCollisions( objects );

  if( m_collisionCallback )
    m_collisionCallback( collisions );

  applyImpulses( collisions );

  for( size_t i = 0; i < 2; ++i )
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
    for( const auto &field: m_fields )
      item->accumulateForce( field );
}

void CPhysicsEngine::applyForces( vector<CObject *> &objects )
{
  for( auto &item: objects )
    item->applyForce();
}

vector<TManifold> CPhysicsEngine::findCollisions( vector<CObject *> &objects )
{
  vector<TManifold> collisions;
  for( auto firstIt = objects.begin(); firstIt != objects.end(); ++firstIt )
    for( auto secondIt = firstIt + 1; secondIt != objects.end(); ++secondIt )
    {
      TManifold collision = (*firstIt)->getManifold( *secondIt );
      if( collision.isValid() )
        collisions.push_back( collision );
    }
  return collisions;
}

void CPhysicsEngine::applyImpulses( vector<TManifold> & )
{

}

void CPhysicsEngine::resolveCollisions( vector<TManifold> & )
{
  // todo
}
