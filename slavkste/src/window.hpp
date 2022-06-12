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

/**
 * Singleton class for managing glut and openGL state-machine.
 */
class CWindow
{
public:
  /**
   * Instance of singleton.
   */
  static CWindow *instance;

  /**
   * Initialises window
   * @param argcPtr program parameter count pointer
   * @param argv program parameter list
   */
  CWindow( int *argcPtr, char *argv[] );

  /**
   * Registers callback for glutDrawEvent request.
   * @tparam type
   * @param cl instance of type
   * @param callback pointer to method on class type
   * @return event id
   */
  template <typename type>
  unsigned int registerDrawEvent( type *cl, void(type::*callback)() );

  /**
   * Register callback to be called after time_ms with values as argument.
   * @tparam type type
   * @tparam types types
   * @param cl instance of type
   * @param callback pointer to method
   * @param values types values
   * @param time_ms timer sleep time in milliseconds
   * @return
   */
  template <typename type, typename ...types>
  unsigned int registerTimerEvent( type *cl, void(type::*callback)( types... ),
                                   types... values, unsigned int time_ms );

  /**
   * Registers callback for glut key press/release event.
   * @tparam type
   * @param cl instance of type
   * @param callback pointer to method on class type
   * @return event id
   */
  template <typename type>
  unsigned int registerKeyEvent( type *cl, void(type::*callback)( unsigned char, int, int ) );

  /**
   * Registers callback for glut mouse press/release event.
   * @tparam type
   * @param cl instance of type
   * @param callback pointer to method on class type
   * @return event id
   */
  template <typename type>
  unsigned int registerMouseButtonEvent( type *cl, void(type::*callback)( int, int, int, int ) );

  /**
   * Registers callback for glut mouse move event.
   * @tparam type
   * @param cl instance of type
   * @param callback pointer to method on class type
   * @return event id
   */
  template <typename type>
  unsigned int registerMotionButtonEvent( type *cl, void(type::*callback)( int, int ) );

  /**
   * Draws line.
   * @param startPoint
   * @param endPoint
   * @param width
   * @param tags
   */
  void drawLine( const TVector<2> &startPoint,
                 const TVector<2> &endPoint,
                 double width, ETag tags = ETag::NONE ) const;

  /**
   * Draws circle with mark at angle. If angle is NAN no mark is drawn.
   * @param centre
   * @param radius
   * @param angle
   * @param tags
   */
  void drawCircle( const TVector<2> &centre,
                   double radius,
                   double angle = NAN,
                   ETag tags = ETag::NONE ) const;

  /**
   * Draws text. Center alignment.
   * @param position
   * @param text
   */
  void drawText( const TVector<2> &position, const std::string &text ) const;

  /**
   * Apply colors and translation for tags.
   * @param tags
   */
  static void applyPenColor( ETag tags );

  /**
   * Restores translation done by applyPenColor.
   * @param tags
   */
  static void restorePenColor( ETag tags );

  /**
   * Moves camera to show area of scene.
   * @param left
   * @param right
   * @param bottom
   * @param top
   */
  void resizeView( double left, double right,
                   double bottom, double top );

  /**
   * @param title new window title
   */
  void changeTitle( const std::string &title );

  /**
   * @return Width and height of screen view.
   */
  [[nodiscard]] TVector<2> getViewSize() const;

  /**
   * Enters processing mainloop for dispatching draws and events.
   * Returns after window close. Or call to glutLeaveMainLoop.
   */
  void mainLoop() const;

private:
  /**
   * Bottom-left of view.
   */
  TVector<2> m_viewOrigin;

  /**
   * Top-right of view.
   */
  TVector<2> m_viewExtreme;

  /**
   * Window size.
   */
  TVector<2> m_windowSize;

  /**
   * Scale of scene. height / width.
   */
  double m_scale = 1;

  /**
   * Handler of native glutDrawEvent.
   */
  static void redrawEventHandler();

  /**
   * Handler of native glutTimerEvent.
   */
  static void timerEventHandler( int );

  /**
   * Handler of native glutResizeEvent.
   */
  static void resizeEventHandler( int, int );

  /**
   * Handler of native glutKeyPressEvent.
   */
  static void keyPressEventHandler( unsigned char, int, int );

  /**
   * Handler of native glutMouseButtonEvent.
   */
  static void mouseButtonEventHandler( int, int, int, int );

  /**
   * Handler of native glutMotionEvent.
   */
  static void mouseMotionEventHandler( int, int );

  /**
   * Resizes window scene to accommodate.
   */
  void resizeWindowAction( int w, int h );

  /**
   * Transforms coordinates from window to scene.
   * @param x, y coordinates
   * @return Resolved coordinates.
   */
  [[nodiscard]] TVector<2> resolveCoordinates( int x, int y ) const;

  /**
   * Callback for redrawEvent.
   */
  std::function<void()> redrawEventCallback;

  /**
   * Callback for timerEvent.
   */
  std::map<int, std::function<void()>> timerEventCallbacks;

  /**
   * Callback for keyPressEvent.
   */
  std::function<void( unsigned char, int, int )> keyPressEventCallback;

  /**
   * Callback for mouseButtonEvent.
   */
  std::function<void( int, int, int, int )> mouseButtonEventCallback;

  /**
   * Callback for moveEvent.
   */
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
