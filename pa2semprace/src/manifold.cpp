#include "manifold.hpp"
#include "object.hpp"


TManifold::TManifold( CPhysicsObject *first, CPhysicsObject *second )
        : first( first ),
          second( second ){}

TManifold::TManifold( CPhysicsObject *first,
                      CPhysicsObject *second,
                      TVector<2> overlapVector,
                      TVector<2> contactPoint )
        : TManifold( first, second )
{
  contacts.push_back( { overlapVector, contactPoint } );
}

TManifold::TManifold( CPhysicsObject *first,
                      CPhysicsObject *second,
                      std::vector<TContactPoint> contacts )
        : first( first ),
          second( second ),
          contacts( move( contacts ) ){}

TManifold::TManifold( CPhysicsObject *first,
                      CPhysicsObject *second,
                      const TContactPoint &contactPoint )
        : TManifold( first, second )
{
  contacts.push_back( contactPoint );
}
