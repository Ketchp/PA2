#pragma once

#include "linearAlgebra.hpp"
#include <vector>
#include <functional>
#include "physicsObject.hpp"


class CPainter
{
public:
  explicit CPainter( std::function<void()> );

  void addPoint( int, int, std::vector<CPhysicsObject *> & );

  void stop();
  void stop( int, int, std::vector<CPhysicsObject *> & );

  TVector<2> lastMousePosition{ NAN, NAN };
  static const double minDrawLength;
  double drawWidth = 8;
  double density = 50;

  void reset();

private:
  void start( int, int, std::vector<CPhysicsObject *> & );

  bool addPoint( const TVector<2> &, std::vector<CPhysicsObject *> & );

  CComplexObject *currentlyDrawn = nullptr;
  std::function<void()> redrawCallback;
};


