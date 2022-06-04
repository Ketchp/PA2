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
  m_window.drawItems( m_objects );
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
      m_painter.stop( x, y );
    else
      nextFrame();
  }
  if( key == 'n' && m_paused )
    nextFrame();
}

void CGame::clickHandler( int button, int state, int x, int y )
{
  m_painter.clickHandler( button, state, x, y, m_objects );
}

void CGame::moveHandler( int x, int y )
{
  m_painter.moveHandler( x, y, m_objects );
}

