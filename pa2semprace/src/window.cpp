#include "window.hpp"
#ifdef __DEBUG
#include <iostream>
#include <cstdio>
#endif

CWindow::CWindow( int *argcPtr, char *argv[] )
{
  assert( !instance && "This implementation allows only one window to exist" != nullptr );
  instance = this;
  CCircle::gluRenderer = gluNewQuadric();


  glutInit( argcPtr, argv );
  glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(320,320);
  m_windowID =  glutCreateWindow("Lighthouse3D - GLUT Tutorial");
  glutSetKeyRepeat( GLUT_KEY_REPEAT_OFF );
  glutIgnoreKeyRepeat( 1 );

  // OpenGL init
  glEnable(GL_DEPTH_TEST);

  // function callback registration
  glutReshapeFunc( &CWindow::resizeEventHandler );

#ifdef __DEBUG
  std::printf( "%s\n", glGetString( GL_VERSION ) );
#endif

}

CWindow::~CWindow()
{
  gluDeleteQuadric( CCircle::gluRenderer );
}


void CWindow::mainLoop() const
{
  // enter GLUT event processing cycle
  glutMainLoop();
}

void CWindow::redrawEventHandler()
{
  instance->redrawEventCallback();
}

void CWindow::timerEventHandler( int id )
{
  instance->timerEventCallbacks[ id ]();
  instance->timerEventCallbacks.erase( id );
}

void CWindow::resizeEventHandler( int w, int h )
{
  instance->resizeWindowAction( w, h );
}

void CWindow::keyPressEventHandler( unsigned char key, int x, int y )
{
  instance->keyPressEventCallback( key, x, y );
}

void CWindow::keyReleaseEventHandler( unsigned char key, int x, int y )
{
  instance->keyReleaseEventCallback( key, x, y );
}

void CWindow::specialKeyPressEventHandler( int key, int x, int y )
{
  instance->specialKeyPressEventCallback( key, x, y );
}

void CWindow::specialKeyReleaseEventHandler( int key, int x, int y )
{
  instance->specialKeyReleaseEventCallback( key, x, y );
}

void CWindow::mouseButtonEventHandler( int button, int state, int x, int y )
{
  instance->mouseButtonEventCallback( button, state, x, y );
}

void CWindow::mouseMotionEventHandler( int x, int y )
{
  instance->mouseMotionEventCallback( x, y );
}

void CWindow::windowCloseEventHandler()
{
  instance->windowCloseEventCallback();
}


void CWindow::resizeWindowAction( int w, int h )
{
  m_windowSize = math::vector{ w, h };
  double viewWidth = m_viewExtreme[ 0 ] - m_viewOrigin[ 0 ];
  double viewHeight = m_viewExtreme[ 1 ] - m_viewOrigin[ 1 ];

  double widthScale = w / viewWidth;
  double heightScale = h / viewHeight;

  double scale = std::min( widthScale, heightScale );

  // Prevent a divide by zero, when window is too short
  // (you can't make a window of zero width).
//  if( h == 0 )
//    h = 1;
//
//  float ratio = (float)w / (float)h;

  // Use the Projection Matrix
  glMatrixMode( GL_PROJECTION );

  // Reset Matrix
  glLoadIdentity();

  // Set the viewport to be the entire window
  glViewport( (GLint)( w - scale * viewWidth ) / 2,
              (GLint)( h - scale * viewHeight ) / 2,
              (GLint)( scale * viewWidth ),
              (GLint)( scale * viewHeight ) );

  // Set the correct perspective.
  //gluPerspective( 60, ratio, 1, 100 );
  gluOrtho2D( m_viewOrigin[ 0 ], m_viewExtreme[ 0 ], m_viewOrigin[ 1 ], m_viewExtreme[ 1 ] );

  // Get Back to the Model view
  glMatrixMode( GL_MODELVIEW );
}

void CWindow::drawItems( const std::vector<std::unique_ptr<CObject>> &itemVector ) const
{
  // Clear Color and Depth Buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Reset transformations
  glLoadIdentity();

  for( const auto &obj: itemVector )
  {
    obj->render();
  }
  glutSwapBuffers();
}

void CWindow::resizeView( double left, double right, double bottom, double top )
{
  m_viewOrigin = { left, bottom };
  m_viewExtreme = { right, top };
  gluOrtho2D( left, right, bottom, top );
}

math::vector CWindow::resolveCoordinates( int x, int y ) const
{
  double viewWidth = m_viewExtreme[ 0 ] - m_viewOrigin[ 0 ];
  double viewHeight = m_viewExtreme[ 1 ] - m_viewOrigin[ 1 ];

  double widthScale = m_windowSize[ 0 ] / viewWidth;
  double heightScale = m_windowSize[ 1 ] / viewHeight;

  double commonScale = std::min( widthScale, heightScale );

  math::vector coords( x, y );
  coords -= m_windowSize / 2;

  coords /= commonScale;

  coords[ 1 ] *= -1;

  coords += ( m_viewOrigin + m_viewExtreme ) / 2;

  return coords;
}

math::vector CWindow::getViewSize() const
{
  return m_viewExtreme - m_viewOrigin;
}


CWindow *CWindow::instance = nullptr;
