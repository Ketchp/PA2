#include "physicsAttributes.hpp"

using namespace std;

TPhysicsAttributes TPhysicsAttributes::lineAttributes( double density, TVector<2> direction )
{
  double length = 2 * direction.norm();
  double mass = length * density;
  return TPhysicsAttributes( mass, mass * length * length / 12 );
}

TPhysicsAttributes TPhysicsAttributes::circleAttributes( double density, double radius )
{
  double mass = M_PI * radius * radius * density;
  return TPhysicsAttributes( mass, mass * radius * radius / 2 );
}

TPhysicsAttributes TPhysicsAttributes::complexObjectAttributes( double density,
                                                                vector<TVector<2>> points,
                                                                TVector<2> centreOfMass )
{
  double mass = 0, segmentMass;
  double angularMass = 0;
  for( auto it = points.begin() + 1; it != points.end(); ++it )
  {
    TVector<2> segmentDirection = *it - *( it - 1 );
    TVector<2> segmentCentre = ( *it + *( it - 1 ) ) / 2;
    double segmentLength = segmentDirection.norm();
    mass += segmentMass = segmentLength * density;
    double localAngularMass = segmentMass * segmentDirection.squareNorm() / 12;
    TVector<2> segmentCentreOffset = centreOfMass - segmentCentre;
    angularMass += localAngularMass + mass * segmentCentreOffset.squareNorm();
  }
  return TPhysicsAttributes( mass, angularMass );
}

TPhysicsAttributes::TPhysicsAttributes( double mass, double angularMass )
  : mass( mass ),
    invMass( 1 / mass ),
    angularMass( angularMass ),
    invAngularMass( 1 / angularMass )
{}
