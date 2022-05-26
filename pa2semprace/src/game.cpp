#include "game.hpp"

using namespace std;

CGame::CGame( int *argcPtr, char *argv[] )
  : m_window( argcPtr, argv ),
    m_levelLoader( "assets/level_1.json" ),
    m_painter( m_objects )
{
  m_levelLoader.loadLevel( m_window, m_engine,
                           m_objects,
                           m_checks );


  m_window.registerDrawEvent( this, &CGame::redraw );
  m_window.registerKeyEvent( this, &CGame::keyPress );
  m_window.registerMouseButtonEvent( &m_painter, &CPainter::clickEvent );
  m_window.registerMotionButtonEvent( &m_painter, &CPainter::moveEvent );
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

void CGame::keyPress( unsigned char key, int, int )
{
  if( key == 'p' )
  {
    m_paused = !m_paused;
    cout << ( m_paused ? "Paused" : "Running" ) << endl;
    if( m_paused )
      m_painter.stop();
    else
    {
      m_painter.restart();
      nextFrame();
    }
  }
  if( key == 'n' && m_paused )
    nextFrame();
}

