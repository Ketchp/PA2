#include "physicsAttributes.hpp"

using namespace std;

TPhysicsAttributes TPhysicsAttributes::rectangleAttributes( double density,
                                                            const TVector<2> &size )
{
  double mass = density * size[ 0 ] * size[ 1 ];
  double angularMass = mass * ( size[ 0 ] * size[ 0 ] + size[ 1 ] * size[ 1 ] ) / 12;
  return { mass, angularMass };
}

TPhysicsAttributes TPhysicsAttributes::circleAttributes( double density, double radius )
{
  double mass = M_PI * radius * radius * density;
  return { mass, mass * radius * radius / 2 };
}

TPhysicsAttributes TPhysicsAttributes::complexObjectAttributes( double width,
                                                                double density,
                                                                vector<TVector<2>> points,
                                                                TVector<2> centreOfMass )
{
  if( points.size() < 2 )
    return { density * M_PI * 100, M_PI * density * 10000 / 2 };

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
  return { mass, angularMass };
}

TPhysicsAttributes::TPhysicsAttributes( double mass, double angularMass )
  : mass( mass ),
    invMass( 1 / mass ),
    angularMass( angularMass ),
    invAngularMass( 1 / angularMass )
{}
