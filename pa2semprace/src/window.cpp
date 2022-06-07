#include "window.hpp"

using namespace std;

CWindow::CWindow( int *argcPtr, char *argv[] )
{
  assert( !instance && "This implementation allows only one window to exist" != nullptr );
  instance = this;

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
  m_windowSize = { (double)w, (double)h };
  double viewWidth = m_viewExtreme[ 0 ] - m_viewOrigin[ 0 ];
  double viewHeight = m_viewExtreme[ 1 ] - m_viewOrigin[ 1 ];

  double widthScale = w / viewWidth;
  double heightScale = h / viewHeight;

  m_scale = std::min( widthScale, heightScale );

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
  glViewport( (GLint)( w - m_scale * viewWidth ) / 2,
              (GLint)( h - m_scale * viewHeight ) / 2,
              (GLint)( m_scale * viewWidth ),
              (GLint)( m_scale * viewHeight ) );

  // Set the correct perspective.
  //gluPerspective( 60, ratio, 1, 100 );
  gluOrtho2D( m_viewOrigin[ 0 ], m_viewExtreme[ 0 ], m_viewOrigin[ 1 ], m_viewExtreme[ 1 ] );

  // Get Back to the Model view
  glMatrixMode( GL_MODELVIEW );
}

void CWindow::resizeView( double left, double right, double bottom, double top )
{
  m_viewOrigin = { left, bottom };
  m_viewExtreme = { right, top };
  gluOrtho2D( left, right, bottom, top );
}

void CWindow::changeTitle( const std::string &title )
{
  glutSetWindowTitle( title.c_str() );
}

TVector<2> CWindow::resolveCoordinates( int x, int y ) const
{
  double viewWidth = m_viewExtreme[ 0 ] - m_viewOrigin[ 0 ];
  double viewHeight = m_viewExtreme[ 1 ] - m_viewOrigin[ 1 ];

  double widthScale = m_windowSize[ 0 ] / viewWidth;
  double heightScale = m_windowSize[ 1 ] / viewHeight;

  double commonScale = std::min( widthScale, heightScale );

  TVector<2> coords{ (double)x, (double)y };
  coords -= m_windowSize / 2;

  coords /= commonScale;

  coords[ 1 ] *= -1;

  coords += ( m_viewOrigin + m_viewExtreme ) / 2;

  return coords;
}

TVector<2> CWindow::getViewSize() const
{
  return m_viewExtreme - m_viewOrigin;
}


void CWindow::drawLine( const TVector<2> &startPoint, const TVector<2> &endPoint,
                        double width, ETag tags )
{

  TVector<2> normal = crossProduct( endPoint - startPoint ).stretchedTo( width );

  glBegin( GL_QUADS );
    if( tags & ETag::WIN_ZONE )
    {
      glTranslatef( 0, 0, -1 );
      glColor4d( 0.1, 0.9, 0.1, 0.7 );
    }

    if( tags & ETag::NO_DRAW_ZONE )
    {
      glTranslatef( 0, 0, -2 );
      glColor4d( 0.9, 0.1, 0.1, 0.7 );
    }
    glVertex2d( (startPoint + normal)[ 0 ], (startPoint + normal)[ 1 ] );
    glVertex2d( (startPoint - normal)[ 0 ], (startPoint - normal)[ 1 ] );
    glVertex2d( (endPoint - normal)[ 0 ], (endPoint - normal)[ 1 ] );
    glVertex2d( (endPoint + normal)[ 0 ], (endPoint + normal)[ 1 ] );

    if( tags & ETag::WIN_ZONE )
    {
      glTranslatef( 0, 0, 1 );
      glColor3d( 1, 1, 1 );
    }
    if( tags & ETag::NO_DRAW_ZONE )
    {
      glTranslatef( 0, 0, 2 );
      glColor3d( 1, 1, 1 );
    }

  glEnd();
}

void CWindow::drawCircle( const TVector<2> &centre, double radius, ETag tags )
{
  drawCircle( centre, radius, 0, M_PI * 2, tags );
}

void CWindow::drawCircle( const TVector<2> &centre, double radius,
                          double startAngle, double endAngle, ETag )
{
  static const size_t slices = 30;
  TVector<2> lever = TVector<2>::canonical( 0, radius ).rotated( startAngle );
  TMatrix<2,2> rotationMatrix =
          TMatrix<2,2>::rotationMatrix2D( ( endAngle - startAngle ) / slices );

  glBegin( GL_TRIANGLE_FAN );
  glVertex2d( centre[ 0 ], centre[ 1 ] );
  glVertex2d( (centre + lever)[ 0 ], (centre + lever)[ 1 ] );
  for( size_t i = 0; i < slices; i++ )
  {
    lever = rotationMatrix * lever;
    glVertex2d( (centre + lever)[ 0 ], (centre + lever)[ 1 ] );
  }

  glEnd();
}

CWindow *CWindow::instance = nullptr;

void CWindow::setColor( float r, float g, float b )
{
  glColor3f( r, g, b );
}

void CWindow::drawText( const TVector<2> &position, const string &text )
{
  auto x = position[ 0 ],
       y = position[ 1 ];

  void *font = GLUT_BITMAP_TIMES_ROMAN_24;

  double textWidth =
          (double)( glutBitmapLength( font,
                reinterpret_cast<const unsigned char *>(text.c_str()) )
           + text.size() - 1 ) / m_scale;
  x -= textWidth / 2;

  for( const auto &c: text )
  {
    glRasterPos2d( x, y );
    glutBitmapCharacter( font, c );
    x += ( glutBitmapWidth( font, c ) + 1 ) / m_scale;
  }
}
