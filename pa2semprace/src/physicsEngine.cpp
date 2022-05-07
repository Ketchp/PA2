#include "physicsEngine.hpp"
#include <numeric>
#include <set>

CForceField::CForceField( std::function<void(CPhysicsObject &)> functor )
  : m_fieldFunctor( std::move(functor) )
{}

void CForceField::applyForce( CPhysicsObject &obj ) const
{
  m_fieldFunctor( obj );
}

CForceField CForceField::gravitationalField( double g )
{
  return CForceField( [g]( CPhysicsObject &object )
  {
    object.m_attributes.forceAccumulator += object.m_attributes.mass * g * math::vector{ 0, -1 };
  } );
}



void CPhysicsEngine::step( std::vector<std::unique_ptr<CObject>> &objects )
{
  for( auto &item: objects )
  {
    auto &physItem = dynamic_cast<CPhysicsObject &>( *item );
    if( physItem.m_attributes.mass == INFINITY )
      continue;
    physItem.m_attributes.forceAccumulator = { 0, 0 };
    for( const auto &field: m_fields )
      field.applyForce( physItem );
    item->m_position += physItem.m_attributes.forceAccumulator / physItem.m_attributes.mass;
  }
  updateObjectsOrder( objects );
  std::vector<std::pair<CObject *, CObject *>> overlappingBBx = getPossibleCollisions( objects );
  calculateDepths( overlappingBBx );
}

void CPhysicsEngine::addField( CForceField field )
{
  m_fields.emplace_back( std::move( field ) );
}

void CPhysicsEngine::updateObjectsOrder( const std::vector<std::unique_ptr<CObject>> &objects )
{
  for( auto &object: objects )
    object->updateBoundingBox();

  m_objectsOrderMinX.resize( objects.size() );
  std::iota( m_objectsOrderMinX.begin(),
             m_objectsOrderMinX.end(),
             0 );
  m_objectsOrderMaxX.resize( objects.size() );
  std::iota( m_objectsOrderMaxX.begin(),
             m_objectsOrderMaxX.end(),
             0 );

  std::sort( m_objectsOrderMinX.begin(), m_objectsOrderMinX.end(),
            [&objects]( const auto &idx_a, const auto &idx_b )
            {
              return objects[ idx_a ]->m_BBx[ 0 ] < objects[ idx_b ]->m_BBx[ 0 ];
            } );
  std::sort( m_objectsOrderMaxX.begin(), m_objectsOrderMaxX.end(),
            [&objects]( const auto &idx_a, const auto &idx_b )
            {
              return objects[ idx_a ]->m_BBx[ 1 ] < objects[ idx_b ]->m_BBx[ 1 ];
            } );
}

std::vector<std::pair<CObject *, CObject *>>
CPhysicsEngine::getPossibleCollisions( std::vector<std::unique_ptr<CObject>> &objects ) const
{
  std::vector<std::pair<CObject *, CObject *>> result;
  std::set<size_t> suspects;
  size_t openIdx = 0,
         closeIdx = 0;
  while( openIdx != objects.size() )
  {
    if( objects[ m_objectsOrderMaxX[ closeIdx ] ]->m_BBx[ 1 ] <
        objects[ m_objectsOrderMinX[ openIdx ] ]->m_BBx[ 0 ] )
    {
      suspects.erase( m_objectsOrderMaxX[ closeIdx++ ] );
      continue;
    }
    for( size_t idx: suspects )
      result.emplace_back( objects[ m_objectsOrderMinX[ openIdx ] ].get(), objects[ idx ].get() );

    suspects.insert( m_objectsOrderMinX[ openIdx++ ] );
  }
  return result;
}

std::vector<CManifold>
CPhysicsEngine::calculateDepths( const std::vector<std::pair<CObject *, CObject *>> &possibleCollisions )
{
  std::vector<CManifold> collisions;
  for( auto [ first, second ]: possibleCollisions )
  {
    CManifold collision = first->getManifold( second );
    if( !collision.overlapVector.isZero() )
      collisions.emplace_back( collision );
  }
  return collisions;
}

CManifold CPhysicsEngine::calculateDepth( const std::pair<CObject *, CObject *> &collision )
{
  return  collision.first->getManifold( collision.second );
}

