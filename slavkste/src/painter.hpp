#pragma once

#include "linearAlgebra.hpp"
#include <vector>
#include <functional>
#include "physicsObject.hpp"

/**
 * @class Class responsible for drawing complex objects to screen and ray-casting.
 */
class CPainter
{
public:
  /**
   * Painter constructor.
   * @param redrawCallback callback called when painter needs to update screen after draw
   */
  explicit CPainter( std::function<void()> redrawCallback );

  /**
   * Tries to add vertex to position (x, y).
   * @param x, y vertex position
   * @param objects vector of blocking objects / storage for drawn objects
   * @return Length of newly drown line, 0 on no draw.
   */
  double addPoint( int x, int y, std::vector<CPhysicsObject *> &objects );

  /**
   * Stop's drawing of object and assigns physics attributes to it.
   */
  void stop();

  /**
   * Finishes drawing on position (x, y) and assigns object attributes to it.
   * @param x, y vertex position
   * @param objects vector of blocking objects / storage for drawn objects
   * @return Length of newly drown line, 0 on no draw.
   */
  double stop( int x, int y, std::vector<CPhysicsObject *> &objects );

  /**
   * Minimal possible line segment that can be drawn.
   */
  static const double minDrawLength;

  /**
   * Width of complex object being drawn.
   */
  double drawWidth = 8;

  /**
   * Density of drawn objects.
   */
  double density = 50;

  /**
   * Stops drawing without spawning object.
   */
  void reset();

private:
  /**
   * Tries to start drawing object on position (x, y).
   * Tries to find valid position in vicinity of (x, y).
   * @param x, y position
   * @param objects vector of blocking objects / storage for drawn objects
   * @return minDrawLen / 2
   */
  double start( int x, int y, std::vector<CPhysicsObject *> &objects );

  /**
   * Tries to create new vertex in direction from lastMousePosition.
   * Drawn line has length between minDrawLen / 2 and minDrawLen.
   * @param direction direction of line
   * @param objects vector of blocking objects / storage for drawn objects
   * @return Length of drawn line.
   */
  double addPoint( const TVector<2> &direction,
                   std::vector<CPhysicsObject *> &objects );

  /**
   * Object that is being currently drawn.
   */
  CComplexObject *currentlyDrawn = nullptr;

  /**
   * Last vertex of object. Non valid if drawing is stopped.
   */
  TVector<2> lastMousePosition{ NAN, NAN };

  /**
   * Callback for redrawing screen.
   */
  std::function<void()> redrawCallback;
};


