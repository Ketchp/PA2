#pragma once
#include "vector.hpp"
#include "matrix.hpp"
#include "physicsAttributes.hpp"
#include "object.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <queue>

class CObject;
class CPhysicsObject;

class CForceField
{
public:
  explicit CForceField( std::function<void(CPhysicsObject &)> );
  void applyForce( CPhysicsObject & ) const;
  static CForceField gravitationalField( double g = 10 );
  std::function<void(CPhysicsObject &)> m_fieldFunctor;
};

struct CManifold
{
  CManifold( CObject *first, CObject *second )
          : first( first ),
            second( second )
  {}

  CManifold( CObject *first, CObject *second,
             math::vector overlapVector,
             math::vector contactPoint )
    : first( first ),
      second( second ),
      overlapVector( overlapVector ),
      contactPoint( contactPoint )
  {}
  CObject *first;
  CObject *second;
  math::vector overlapVector;
  math::vector contactPoint;
};

class CPhysicsEngine
{
public:
  void step( std::vector<std::unique_ptr<CObject>> & );
  void addField( CForceField );
private:
  std::vector<CManifold> getCollisions( const CObject &,
                                        std::vector<std::unique_ptr<CObject>>::const_iterator,
                                        std::vector<std::unique_ptr<CObject>>::const_iterator );
  std::vector<std::pair<CObject *, CObject *>> getPossibleCollisions( std::vector<std::unique_ptr<CObject>> & ) const;
  void updateObjectsOrder( const std::vector<std::unique_ptr<CObject>> & );
  static std::vector<CManifold> calculateDepths( const std::vector<std::pair<CObject *, CObject *>> & );
  static CManifold calculateDepth( const std::pair<CObject *, CObject *> & );
  std::vector<CForceField> m_fields;
  std::vector<size_t> m_objectsOrderMinX;
  std::vector<size_t> m_objectsOrderMaxX;
};
