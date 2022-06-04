#include "painter.hpp"
#include <iostream>

using namespace std;

CPainter::CPainter( std::function<void()> callback )
  : redrawCallback( move( callback ) )
{}

void CPainter::clickHandler( int button, int state, int x, int y,
                             vector<CObject *> &objects )
{
  if( button == GLUT_LEFT_BUTTON )
  {
    if( state == GLUT_DOWN )
      start( x, y, objects );
    else if( state == GLUT_UP )
      stop( x, y );
  }
  cout << "Click: [ " << x << ", " << y << " ]" << endl;
}

void CPainter::moveHandler( int x, int y, vector<CObject *> &objects )
{
  cout << "Move: [" << x << ", " << y << " ]" << endl;
  if( lastMousePosition.distance( { (double)x, (double)y } ) > 20 )
    addPoint( x, y );
}

void CPainter::start( int x, int y, vector<CObject *> &objects )
{
  lastMousePosition = { (double)x, (double)y };
  currentlyDrawn = new CComplexObject( 0, { lastMousePosition }, HUGE_VAL );
  objects.push_back( currentlyDrawn );
}

void CPainter::addPoint( int x, int y )
{
  lastMousePosition = { (double)x, (double)y };
  currentlyDrawn->addVertex( lastMousePosition );
  redrawCallback();
}

void CPainter::stop( int x, int y )
{
  if( lastMousePosition.distance( { (double)x, (double)y } ) > 10 )
    addPoint( x, y );
  lastMousePosition = { NAN, NAN };
  currentlyDrawn = nullptr;
}

