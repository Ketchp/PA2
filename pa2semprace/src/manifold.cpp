#include "manifold.hpp"
#include "object.hpp"

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

TManifold::TManifold( CObject *first, CObject *second, std::vector<TContactPoint> contacts )
  : first( dynamic_cast<CPhysicsObject *>( first ) ),
    second( dynamic_cast<CPhysicsObject *>( second ) ),
    contacts( move( contacts ) )
{}

TManifold::TManifold( CObject *first, CObject *second,
                      const TContactPoint &contactPoint )
  : TManifold( first, second )
{
  contacts.push_back( contactPoint );
}
