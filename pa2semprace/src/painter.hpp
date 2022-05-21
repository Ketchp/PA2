#pragma once
#include "linearAlgebra.hpp"
#include <vector>
#include "object.hpp"

class CPainter
{
public:
  explicit CPainter( std::vector<CObject *> & );
  void moveEvent( int, int );
  void clickEvent( int, int, int, int );
  void stop();
  void restart();
private:
  TVector<2> lastMousePosition{ NAN, NAN };
  std::vector<CObject *> &m_objects;
  CObject *currentlyDrawn = nullptr;
};


