#include "window.hpp"
#ifdef __DEBUG
#include <iostream>
#include <cstdio>
#endif

void window::init()
{
  assert( !instance && "This implementation allows only one window to exist" != nullptr );
  instance = this;

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(320,320);
  glutCreateWindow("Lighthouse3D - GLUT Tutorial");
  glutSetKeyRepeat( GLUT_KEY_REPEAT_OFF );
  glutIgnoreKeyRepeat( 1 );

  // OpenGL init
  glEnable(GL_DEPTH_TEST);

  // function callback registration

  glutReshapeFunc( &window::resizeEventHandler );
  resizeEventCallback = [this]( int w, int h ){ this->resizeWindowAction( w, h ); };

#ifdef __DEBUG
  std::printf( "%s\n", glGetString( GL_VERSION ) );
#endif

}

void window::mainLoop() const
{
  // enter GLUT event processing cycle
  glutMainLoop();
}

void window::redrawEventHandler()
{
  instance->redrawEventCallback();
}

void window::idleEventHandler()
{
  instance->idleEventCallback();
}

void window::timerEventHandler( int id )
{
  instance->timerEventCallbacks[ id ]();
  // todo recycle ids
}

void window::resizeEventHandler( int h, int w )
{
  instance->resizeEventCallback( h, w );
}

void window::keyPressEventHandler( unsigned char key, int x, int y )
{
  instance->keyPressEventCallback( key, x, y );
}

void window::keyReleaseEventHandler( unsigned char key, int x, int y )
{
  instance->keyReleaseEventCallback( key, x, y );
}

void window::specialKeyPressEventHandler( int key, int x, int y )
{
  instance->specialKeyPressEventCallback( key, x, y );
}

void window::specialKeyReleaseEventHandler( int key, int x, int y )
{
  instance->specialKeyReleaseEventCallback( key, x, y );
}

void window::windowCloseEventHandler()
{
  instance->windowCloseEventCallback();
}

void window::resizeWindowAction( int w, int h )
{
  #ifdef __DEBUG
  std::cout << "Resizing: " << w << ", " << h << std::endl;
  #endif
  // Prevent a divide by zero, when window is too short
  // (you can't make a window of zero width).
  if( h == 0 )
    h = 1;

  float ratio = (float)w / (float)h;

  // Use the Projection Matrix
  glMatrixMode( GL_PROJECTION );

  // Reset Matrix
  glLoadIdentity();

  // Set the viewport to be the entire window
  glViewport( 0, 0, w, h );

  // Set the correct perspective.
  gluPerspective( 60, ratio, 1, 100 );

  // Get Back to the Model view
  glMatrixMode( GL_MODELVIEW );
}

void window::drawItems( const std::vector<std::unique_ptr<object>> &itemVector ) const
{
  // Clear Color and Depth Buffers
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Reset transformations
  glLoadIdentity();

  // Set the camera
  gluLookAt( cameraPosition.x, cameraPosition.y, cameraPosition.z,
             cameraPosition.x, cameraPosition.y, cameraPosition.z - 1,
             0, 1, 0 );


  for( const auto &obj: itemVector )
  {
    obj->render();
  }
  glutSwapBuffers();
}

window *window::instance = nullptr;
