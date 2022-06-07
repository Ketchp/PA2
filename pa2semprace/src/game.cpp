#include "game.hpp"
#include "text.hpp"

using namespace std;

CGame::CGame( int *argcPtr, char *argv[] )
  : m_window( argcPtr, argv ),
    m_levelLoader( "assets/level_1.json" ),
    m_painter( [this](){ redraw(); } )
{
  m_levelLoader.loadLevel( m_window, m_engine,
                           m_objects,
                           m_checks,
                           m_painter );


  m_window.registerDrawEvent( this, &CGame::redraw );
  m_window.registerKeyEvent( this, &CGame::keyPress );
  m_window.registerMouseButtonEvent( this, &CGame::clickHandler );
  m_window.registerMotionButtonEvent( this, &CGame::moveHandler );
  m_engine.registerCollisionCallback( [this]( const vector<TManifold> &collisions )
                                      { return checkCollisions( collisions ); } );
}

CGame::~CGame()
{
  for( const auto &obj: m_objects )
    delete obj;
}

void CGame::nextFrame()
{
  if( !m_paused )
  {
    cout << "Step" << endl;
    m_engine.step( m_objects, 1. / framerate );
    m_window.registerTimerEvent( this, &CGame::nextFrame, 1000 / framerate );
  }
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
  cout << "Key: " << key << endl;
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
  {
    m_engine.step( m_objects, 1. / framerate );
    redraw(); // todo: remove 'n': only for debug
  }
  keyChecks( key );
}

void CGame::keyChecks( unsigned char key )
{
  for( const auto &check: m_checks )
  {
    if( check.type != ECheckType::keyPress )
      continue;
    if( key != check.spec.button )
      continue;
    cout << "Match" << endl;
    m_paused = true;
    m_levelLoader.loadLevel( m_window, m_engine, m_objects,
                             m_checks, m_painter, check.action );
    redraw();
    break;
  }
}

void CGame::clickHandler( int button, int state, int x, int y )
{
  cout << "Click: [ " << x << ", " << y << " ]" << endl;
  if( !m_engine.frame )
    for( auto it = m_objects.begin(); it != m_objects.end(); )
    {
      if( dynamic_cast<CText *>(*it) )
        it = m_objects.erase( it );
      else
        ++it;
    }

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

bool CGame::checkCollisions( const vector<TManifold> &collisions )
{
  return any_of( collisions.begin(), collisions.end(), [this]( const auto &collision )
  {
    return checkCollision( collision );
  } );
}

bool CGame::checkCollision( const TManifold &collision )
{
  for( const auto &check: m_checks )
  {
    if( check.type != ECheckType::collision )
      continue;

    bool result;
    using collisionType = TCheck::subType::collisionType::collisionCondition;
    switch( check.spec.collisionType.collisionCond )
    {
    case collisionType::TagTag:
      result = checkCollisionTagTag( check, collision );
      break;
    case collisionType::TagId:
      result = checkCollisionTagId( check, collision );
      break;
    case collisionType::IdId:
      result = checkCollisionIdId( check, collision );
    }
    if( !result )
      continue;
    m_paused = true;
    m_levelLoader.loadLevel( m_window, m_engine, m_objects,
                             m_checks, m_painter, check.action );
    redraw();
    return true;
  }
  return false;
}

bool CGame::checkCollisionTagTag( const TCheck &check, const TManifold &collision )
{
  return ( collision.first->tags & check.spec.collisionType.first.tag &&
           collision.second->tags & check.spec.collisionType.second.tag ) ||
         ( collision.first->tags & check.spec.collisionType.second.tag &&
           collision.second->tags & check.spec.collisionType.first.tag );
}

bool CGame::checkCollisionTagId( const TCheck &check, const TManifold &collision )
{
  return ( collision.first->m_id == check.spec.collisionType.second.id &&
           collision.second->tags & check.spec.collisionType.first.tag ) ||
         ( collision.second->m_id == check.spec.collisionType.second.id &&
           collision.first->tags & check.spec.collisionType.first.tag );
}

bool CGame::checkCollisionIdId( const TCheck &check, const TManifold &collision )
{
  return ( collision.first->m_id == check.spec.collisionType.first.id &&
           collision.second->m_id == check.spec.collisionType.second.id ) ||
         ( collision.first->m_id == check.spec.collisionType.second.id &&
           collision.second->m_id == check.spec.collisionType.first.id );
}
