#include "painter.hpp"
#include <iostream>

CPainter::CPainter( std::vector<CObject *> &objects )
  : m_objects( objects )
{}

void CPainter::moveEvent( int x, int y )
{

}

void CPainter::clickEvent( int button, int state, int x, int y )
{
  if( button == GLUT_LEFT_BUTTON )
  {
    if( state == GLUT_DOWN )
    {
      lastMousePosition = { (double)x, (double)y };
    }
  }
  std::cout << "Click: [ " << x << ", " << y << " ]" << std::endl;
}
