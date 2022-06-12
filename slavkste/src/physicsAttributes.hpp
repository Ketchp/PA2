#pragma once

#include "linearAlgebra.hpp"
#include <vector>


/**
 * Aggregate structure for storing properties of physics objects.
 */
struct TPhysicsAttributes
{
  /**
   * Construct attributes with mass and angularMass.
   * @param mass
   * @param angularMass
   */
  TPhysicsAttributes( double mass, double angularMass );

  /**
   * Calculates physics attributes for rectangle with density and size.
   * Angular mass is calculated around centre axis.
   * @param density
   * @param size
   * @return Physics attributes of rectangle.
   */
  static TPhysicsAttributes rectangleAttributes( double density, const TVector<2> &size );

  /**
   * Calculates physics attributes for circle with density and radius.
   * Angular mass is calculated around centre axis.
   * @param density
   * @param radius
   * @return Physics attributes of circle.
   */
  static TPhysicsAttributes circleAttributes( double density, double radius );

  /**
   * Calculates physics attributes for complex object with density, width and points.
   * Angular mass is calculated around coordinates origin.
   * If only one point is present, calculation collapses to circle attributes.
   * @param width
   * @param density
   * @param points
   * @return Physics attributes of complex object.
   */
  static TPhysicsAttributes complexObjectAttributes( double width,
                                                     double density,
                                                     std::vector<TVector<2>> points );

  /**
   * Translational velocity of object.
   */
  TVector<2> velocity;

  /**
   * Angular velocity of object. ( Clockwise )
   */
  double angularVelocity = 0;

  /**
   * Object mass.
   */
  double mass;

  /**
   * Object inverse mass. ( 1 / mass )
   */
  double invMass;

  /**
   * Object angular mass.
   */
  double angularMass;

  /**
   * Object inverse angular mass. ( 1 / angular mass )
   */
  double invAngularMass;

  /**
   * Accumulator for force.
   */
  TVector<2> forceAccumulator;

  /**
   * Accumulator for moment. ( Torque )
   */
  double momentAccumulator = 0;

  /**
   * Object elasticity. Percentage of energy preserved on normal collision.
   */
  double elasticity = 0.8;

  /**
   * Object friction coefficient. Upper limit to friction force.
   */
  double frictionCoefficient = 0.5;

  /**
   * Accumulator for object collision damage.
   */
  double integrity = 1;
};
