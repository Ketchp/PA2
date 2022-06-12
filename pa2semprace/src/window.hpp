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
  unsigned int registerTimerEvent( type *cl, void(type::*callback)( types... ),
                                   types... values, unsigned int time_ms );

  template <typename type>
  unsigned int registerKeyEvent( type *cl, void(type::*callback)( unsigned char, int, int ) );

  template <typename type>
  unsigned int registerMouseButtonEvent( type *cl, void(type::*callback)( int, int, int, int ) );

  template <typename type>
  unsigned int registerMotionButtonEvent( type *cl, void(type::*callback)( int, int ) );

  void drawLine( const TVector<2> &startPoint,
                 const TVector<2> &endPoint,
                 double, ETag = ETag::NONE ) const;

  void drawCircle( const TVector<2> &centre,
                   double radius,
                   double angle = NAN,
                   ETag = ETag::NONE ) const;

  void drawText( const TVector<2> &position, const std::string &text ) const;

  static void applyPenColor( ETag );

  static void restorePenColor( ETag );


  void resizeView( double left, double right,
                   double bottom, double top );

  void changeTitle( const std::string & );

  [[nodiscard]] TVector<2> getViewSize() const;

  void mainLoop() const;

private:
  TVector<2> m_viewOrigin;
  TVector<2> m_viewExtreme;
  TVector<2> m_windowSize;
  double m_scale = 1;

  static void redrawEventHandler();

  static void timerEventHandler( int );

  static void resizeEventHandler( int, int );

  static void keyPressEventHandler( unsigned char, int, int );

  static void mouseButtonEventHandler( int, int, int, int );

  static void mouseMotionEventHandler( int, int );

  void resizeWindowAction( int w, int h );

  [[nodiscard]] TVector<2> resolveCoordinates( int x, int y ) const;

  std::function<void()> redrawEventCallback;
  std::map<int, std::function<void()>> timerEventCallbacks;
  std::function<void( unsigned char, int, int )> keyPressEventCallback;
  std::function<void( int, int, int, int )> mouseButtonEventCallback;
  std::function<void( int, int )> mouseMotionEventCallback;
};


template <typename type>
unsigned int
CWindow::registerDrawEvent( type *cl, void(type::*callback)() )
{
  redrawEventCallback = [ cl, callback ](){ ( cl->*callback )(); };
  glutDisplayFunc( &CWindow::redrawEventHandler );
  return 0;
}

template <typename type, typename ...types>
unsigned int
CWindow::registerTimerEvent( type *cl,
                             void(type::*callback)( types... ),
                             types... values,
                             unsigned int time_ms )
{
  static int timerHandlerId = 0;
  timerEventCallbacks.emplace( timerHandlerId, [ cl, callback, values... ](){ ( cl->*callback )( values... ); } );
  glutTimerFunc( time_ms, &CWindow::timerEventHandler, timerHandlerId++ );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerKeyEvent( type *cl, void(type::*callback)( unsigned char, int, int ) )
{
  keyPressEventCallback = [ cl, callback, this ]( unsigned char key, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    ( cl->*callback )( key, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutKeyboardFunc( &CWindow::keyPressEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerMouseButtonEvent( type *cl, void(type::*callback)( int, int, int, int ) )
{
  mouseButtonEventCallback = [ cl, callback, this ]( int button, int state, int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    ( cl->*callback )( button, state, relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutMouseFunc( &CWindow::mouseButtonEventHandler );
  return 0;
}

template <typename type>
unsigned int
CWindow::registerMotionButtonEvent( type *cl, void(type::*callback)( int, int ) )
{
  mouseMotionEventCallback = [ cl, callback, this ]( int x, int y )
  {
    TVector<2> relativeCoord = resolveCoordinates( x, y );
    ( cl->*callback )( relativeCoord[ 0 ], relativeCoord[ 1 ] );
  };
  glutMotionFunc( &CWindow::mouseMotionEventHandler );
  return 0;
}
