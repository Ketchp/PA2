#pragma once
#include "linearAlgebra.hpp"
#include <vector>
#include <functional>
#include "object.hpp"

class CGame;

class CPainter
{
public:
  explicit CPainter( std::function<void()> );
  void start( int, int, std::vector<CObject *> & );
  void addPoint( int, int, const std::vector<CObject *> & );
  void stop( int, int, const std::vector<CObject *> & );

  TVector<2> lastMousePosition{ NAN, NAN };
  static const double minDrawLength;
  double drawWidth = 8;
  double density = 50;
private:
  CComplexObject *currentlyDrawn = nullptr;
  std::function<void()> redrawCallback;
};


