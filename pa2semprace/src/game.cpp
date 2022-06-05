#include "game.hpp"

using namespace std;

int frame;

CGame::CGame( int *argcPtr, char *argv[] )
  : m_window( argcPtr, argv ),
    m_levelLoader( "assets/level_1.json" ),
    m_painter( [this](){ redraw(); } )
{
  m_levelLoader.loadLevel( m_window, m_engine,
                           m_objects,
                           m_checks );


  m_window.registerDrawEvent( this, &CGame::redraw );
  m_window.registerKeyEvent( this, &CGame::keyPress );
  m_window.registerMouseButtonEvent( this, &CGame::clickHandler );
  m_window.registerMotionButtonEvent( this, &CGame::moveHandler );
}

CGame::~CGame()
{
  for( const auto &obj: m_objects )
    delete obj;
}

void CGame::nextFrame()
{
  cout << "Step" << endl;
  m_engine.step( m_objects, 1. / framerate );
  ++frame;
  if( !m_paused )
    m_window.registerTimerEvent( this, &CGame::nextFrame, 1000 / framerate );
  redraw();
}

void CGame::redraw()
{
  // Clear Color and Depth Buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Reset transformations
  glLoadIdentity();

  for( const auto &obj: m_objects )
    obj->render( m_window );

  glutSwapBuffers();
}

void CGame::mainLoop()
{
  m_window.mainLoop();
}

void CGame::keyPress( unsigned char key, int x, int y )
{
  if( key == 'p' )
  {
    m_paused = !m_paused;
    cout << ( m_paused ? "Paused" : "Running" ) << endl;
    if( m_paused )
      m_painter.stop( x, y, m_objects );
    else
      nextFrame();
  }
  if( key == 'n' && m_paused )
    nextFrame();
}

void CGame::clickHandler( int button, int state, int x, int y )
{
  cout << "Click: [ " << x << ", " << y << " ]" << endl;
  if( button == GLUT_LEFT_BUTTON )
  {
    if( state == GLUT_DOWN )
    {
      m_painter.start( x, y, m_objects );
      m_paused = true;
    }
    else if( state == GLUT_UP )
    {
      m_painter.stop( x, y, m_objects );
      m_paused = false;
      nextFrame();
    }
  }
}

void CGame::moveHandler( int x, int y )
{
  cout << "Move: [" << x << ", " << y << " ]" << endl;
  if( m_painter.lastMousePosition.distance( { (double)x, (double)y } ) > CPainter::minDrawLength )
    m_painter.addPoint( x, y, m_objects );
}

