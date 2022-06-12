#include "manifold.hpp"


TManifold::TManifold( CPhysicsObject *first, CPhysicsObject *second )
  : first( first ),
    second( second )
{}

TManifold::TManifold( CPhysicsObject *first,
                      CPhysicsObject *second,
                      TVector<2> overlapVector,
                      TVector<2> contactPoint )
  : TManifold( first, second )
{
  contacts.push_back( { overlapVector, contactPoint } );
}

TManifold::TManifold( CPhysicsObject *first_i,
                      CPhysicsObject *second_i,
                      std::vector<TContactPoint> allContacts )
  : first( first_i ),
    second( second_i ),
    contacts( move( allContacts ) )
{
  if( contacts.empty() )
    first = second = nullptr;
}

TManifold::TManifold( CPhysicsObject *first_i,
                      CPhysicsObject *second_i,
                      const TContactPoint &contactPoint )
  : TManifold( first_i, second_i )
{
  if( contactPoint.contactPoint )
    contacts.push_back( contactPoint );
  else
    first = second = nullptr;
}
