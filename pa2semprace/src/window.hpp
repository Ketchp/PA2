#pragma once
#include "linearAlgebra.hpp"
#include "tags.hpp"
#include <GL/freeglut.h>
#include <list>
#include <vector>
#include <functional>
#include <map>
#include <utility>
#include <memory>
#include <cassert>

class CWindow
{
public:
  static CWindow *instance;

  CWindow( int *argcPtr, char *argv[] );

  template <typename type>
  unsigned int registerDrawEvent( type *cl, void(type::*callback)() );

  template <typename type, typename ...types>
  unsigned int registerTimerEvent( type *cl, void(type::*callback)( types... ), types... values, int time_ms );

  template <typename type>
  unsigned int registerKeyEvent( type *cl, void(type::*callback)( unsigned char, int, int ) );

  template <typename type>
  unsigned int registerKeyUpEvent( type *cl, void(type::*callback)( unsigned char, int, int ) );

  template <typename type>
  unsigned int registerSpecialKeyEvent( type *cl, void(type::*callback)( int, int, int ) );

  template <typename type>
  unsigned int registerSpecialKeyUpEvent( type *cl, void(type::*callback)( int, int, int ) );

  template <typename type>
  unsigned int registerMouseButtonEvent( type *cl, void(type::*callback)( int, int, int, int ) );

  template <typename type>
  unsigned int registerMotionButtonEvent( type *cl, void(type::*callback)( int, int ) );

  template <typename type>
  unsigned int registerWindowCloseEvent( type *cl, void(type::*callback)() );

  void drawLine( const TVector<2> &startPoint, const TVector<2> &endPoint,
                 double, ETag = ETag::NONE );
  void drawCircle( const TVector<2> &centre, double radius,
                   ETag = ETag::NONE );
  void drawCircle( const TVector<2> &centre, double radius,
                   double startAngle, double endAngle,
                   ETag = ETag::NONE );

  void drawText( const TVector<2> &position, const std::string &text );

  void setColor( float, float, float );

  void resizeView( double left,
                   double right,
                   double bottom,
                   double top );

  void changeTitle( const std::string & );

  TVector<2> getViewSize() const;

  void mainLoop() const;
  int m_windowID;
private:
  TVector<2> m_viewOrigin;
  TVector<2> m_viewExtreme;
  TVector<2> m_windowSize;
  double m_scale;
  static void redrawEventHandler();
  static void timerEventHandler( int );
  static void resizeEventHandler( int, int );
  static void keyPressEventHandler( unsigned char, int, int );
  static void keyReleaseEventHandler( unsigned char, int, int );
  static void specialKeyPressEventHandler( int, int, int );
  static void specialKeyReleaseEventHandler( int, int, int );
  static void mouseButtonEventHandler( int, int, int, int );
  static void mouseMotionEventHandler( int, int );
  static void windowCloseEventHandler();

  void resizeWindowAction( int w, int h );
  TVector<2> resolveCoordinates( int x, int y ) const;

  std::function<void()> redrawEventCallback;
  std::map< int, std::function<void()> > timerEventCallbacks;
  std::function<void( unsigned char, int, int )> keyPressEventCallback;
  std::function<void( unsigned char, int, int )> keyReleaseEventCallback;
  std::function<void( int, int, int )> specialKeyPressEventCallback;
  std::function<void( int, int, int )> specialKeyReleaseEventCallback;
  std::function<void( int, int, int, int )> mouseButtonEventCallback;
  std::function<void( int, int )> mouseMotionEventCallback;
  std::function<void()> windowCloseEventCallback;
};


template <typename type>
unsigned int
CWindow::registerDrawEvent( type *cl, void(type::*callback)() )
{
  redrawEventCallback = [cl, callback](){ (cl->*callback)(); };
  glutDisplayFunc( &CWindow::redrawEventHandler );
  return 0;
}

template <typename type, typename ...types>
unsigned int
CWindow::registerTimerEvent( type *cl,
                             void(type::*callback)( types... ),
                             types... values,
                             int time_ms )
{
  static int timerHandlerId = 0;
  timerEventCallbacks.emplace( timerHandlerId, [cl, callback, values... ](){ (cl->*callback)( values... ); } );
  glutTimerFunc( time_ms, &CWindow::timerEventHandler, timerHandlerId++ );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerKeyEvent( type *cl, void(type::*callback)( unsigned char, int, int ) )
{
  keyPressEventCallback = [cl, callback, this]( unsigned char key, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    (cl->*callback)( key, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutKeyboardFunc( &CWindow::keyPressEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerKeyUpEvent( type *cl, void(type::*callback)( unsigned char, int, int ) )
{
  keyReleaseEventCallback = [cl, callback, this]( unsigned char key, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    (cl->*callback)( key, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutKeyboardUpFunc( &CWindow::keyReleaseEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerSpecialKeyEvent( type *cl, void(type::*callback)( int, int, int ) )
{
  specialKeyPressEventCallback = [cl, callback, this]( int key, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    (cl->*callback)( key, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutSpecialFunc( &CWindow::specialKeyPressEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerSpecialKeyUpEvent( type *cl, void(type::*callback)( int, int, int ) )
{
  specialKeyReleaseEventCallback = [cl, callback, this]( int key, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    (cl->*callback)( key, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutSpecialUpFunc( &CWindow::specialKeyReleaseEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerMouseButtonEvent( type *cl, void(type::*callback)( int, int, int, int ) )
{
  mouseButtonEventCallback = [cl, callback, this]( int button, int state, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    (cl->*callback)( button, state, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutMouseFunc( &CWindow::mouseButtonEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerMotionButtonEvent( type *cl, void(type::*callback)( int, int ) )
{
  mouseMotionEventCallback = [cl, callback, this]( int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    (cl->*callback)( relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutMotionFunc( &CWindow::mouseMotionEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerWindowCloseEvent( type *cl, void(type::*callback)() )
{
  windowCloseEventCallback = [cl, callback](){ (cl->*callback)(); };
  glutCloseFunc( &CWindow::windowCloseEventHandler );
  return 0;
}
