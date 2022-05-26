#include "painter.hpp"
#include <iostream>

using namespace std;

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
  cout << "Click: [ " << x << ", " << y << " ]" << endl;
}

void CPainter::stop()
{

}

void CPainter::restart()
{

}
