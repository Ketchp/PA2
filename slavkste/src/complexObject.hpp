#pragma once

#include "physicsObject.hpp"

/**
 * Class for representing solid line strip with fixed joints.
 * Object position is relative to centre of mass.
 */
class CComplexObject : public CPhysicsObject
{
public:
  /**
   * Constructs empty complex object with no vertices.
   * Density is initialised to HUGE_VAL
   * @param width half width of lines( radius of joint circles )
   */
  explicit CComplexObject( double width );

  /**
   * Recalculates objects centre of mass, position, mass, angular mass
   * @param density new density of object
   */
  void spawn( double density = HUGE_VAL );

  /**
   * Renders complex object to window.
   * Line strip with circle at all vertices.
   * @param window target window
   */
  void render( CWindow &window ) const override;

  /**
   * Method for performing rotation on complex object.
   * Updates all vertices to rotate around centre of mass. Leaves rotation as 0.
   * Should not be called before spawn.
   * @param angle angle to rotate object
   * @return CPhysicsObject instance.
   */
  CPhysicsObject &rotate( double angle ) override;

  /**
   * Calculates collision manifold with other object.
   * @param other physics object
   * @return Collision manifold.
   */
  TManifold getManifold( CPhysicsObject *other ) override;

  /**
   * Calculates collision manifold with rectangle.
   * @param rectangle
   * @return Collision manifold.
   */
  TManifold getManifold( CRectangle *rectangle ) override;

  /**
   * Calculates collision manifold with circle.
   * @param circle circle
   * @return Collision manifold.
   */
  TManifold getManifold( CCircle *circle ) override;

  /**
   * Calculates collision manifold with other complex object.
   * @param other other complex object
   * @return Collision manifold.
   */
  TManifold getManifold( CComplexObject *other ) override;

  /**
   * Calculates collisions with circle.
   * @param centre circle centre
   * @param radius circle radius
   * @return Vector of collision points.
   */
  [[nodiscard]] std::vector<TContactPoint> getCircleCollision( const TVector<2> &centre,
                                                               double radius ) const;

  /**
   * Calculates node collisions with other complex.
   * @param complexObject other complex object
   * @param node node position of complex object
   * @return Vector of collision points.
   */
  std::vector<TContactPoint> getNodeCollisions( CComplexObject *complexObject,
                                                const TVector<2> &node ) const;

  /**
   * Calculates largest distance ray can travel from position in direction.
   * @param position position
   * @param direction direction
   * @return Positive double.
   */
  [[nodiscard]] double rayTrace( const TVector<2> &position,
                                 const TVector<2> &direction ) const override;

  /**
   * Adds new vertex to object.
   * @param vertex position relative to coordinates origin.
   */
  void addVertex( const TVector<2> &vertex );

  /**
   * Calculates centre of mass of homogenous line strip.
   * @param vertices vector of vertices
   * @return Position of centre of mass.
   */
  static TVector<2> calculateCentreOfMass( const std::vector<TVector<2>> &vertices );

  /**
   * Half-width of lines, radius of joints.
   */
  double m_width;

  /**
   * Longest line in line strip.
   * Allows optimisations when calculating collisions.
   */
  double m_longest = 0;
private:
  /**
   * Object vertices.
   */
  std::vector<TVector<2>> m_vertices;
};

