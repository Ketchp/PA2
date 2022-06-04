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
  void clickHandler( int, int, int, int, std::vector<CObject *> & );
  void moveHandler( int, int, std::vector<CObject *> & );
  void start( int, int, std::vector<CObject *> & );
  void addPoint( int, int );
  void stop( int, int );
private:
  TVector<2> lastMousePosition{ NAN, NAN };
  CComplexObject *currentlyDrawn = nullptr;
  std::function<void()> redrawCallback;
};


