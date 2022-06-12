#include "forceField.hpp"


using namespace std;

CForceField::CForceField( function<void( CPhysicsObject & )> functor )
  : m_fieldFunctor( move( functor ) )
{}

void CForceField::applyForce( CPhysicsObject &obj ) const
{
  m_fieldFunctor( obj );
}

CForceField CForceField::gravitationalField( double g )
{
  return CForceField( [ g ]( CPhysicsObject &object )
                      {
                        object.m_attributes.forceAccumulator +=
                                g * object.m_attributes.mass * TVector<2>{ 0, -1 };
                      } );
}
