#pragma once
#include <GL/freeglut.h>
#include "object.hpp"
#include "helpers.hpp"
#include <list>
#include <vector>
#include <functional>
#include <map>
#include <utility>
#include <memory>
#include "cassert"

class window
{
public:
  static window *instance;

  void init();

  template <typename type>
  unsigned int registerDrawEvent( type *cl, void(type::*callback)() )
  {
    redrawEventCallback = [cl, callback](){ (cl->*callback)(); };
    glutDisplayFunc( &window::redrawEventHandler );
    return 0;
  }

  template <typename type>
  unsigned int registerIdleEvent( type *cl, void(type::*callback)() )
  {
    idleEventCallback = [cl, callback](){ (cl->*callback)(); };
    glutIdleFunc( &window::idleEventHandler );
    return 0;
  }

  template <typename type, typename ...types>
  unsigned int registerTimerEvent( type *cl, void(type::*callback)( types... ), types... values, int time_ms )
  {
    timerEventCallbacks.emplace( timerHandlerId, [cl, callback, values... ](){ (cl->*callback)( values... ); } );
    glutTimerFunc( time_ms, &window::timerEventHandler, timerHandlerId++ );
    return 0;
  }

//  template <typename type>
//  unsigned int registerResizeEvent( type *cl, void(type::*)( int, int ) );

  template <typename type>
  unsigned int registerKeyEvent( type *cl, void(type::*callback)( unsigned char, int, int ) )
  {
    keyPressEventCallback = [cl, callback]( unsigned char key, int x, int y ){ (cl->*callback)( key, x, y ); };
    glutKeyboardFunc( &window::keyPressEventHandler );
    return 0;
  }

  template <typename type>
  unsigned int registerKeyUpEvent( type *cl, void(type::*callback)( unsigned char, int, int ) )
  {
    keyReleaseEventCallback = [cl, callback]( unsigned char key, int x, int y ){ (cl->*callback)( key, x, y ); };
    glutKeyboardUpFunc( &window::keyReleaseEventHandler );
    return 0;
  }

  template <typename type>
  unsigned int registerSpecialKeyEvent( type *cl, void(type::*callback)( int, int, int ) )
  {
    specialKeyPressEventCallback = [cl, callback]( int key, int x, int y ){ (cl->*callback)( key, x, y ); };
    glutSpecialFunc( &window::specialKeyPressEventHandler );
    return 0;
  }

  template <typename type>
  unsigned int registerSpecialKeyUpEvent( type *cl, void(type::*callback)( int, int, int ) )
  {
    specialKeyReleaseEventCallback = [cl, callback]( int key, int x, int y ){ (cl->*callback)( key, x, y ); };
    glutSpecialUpFunc( &window::specialKeyReleaseEventHandler );
    return 0;
  }

  template <typename type>
  unsigned int registerWindowCloseEvent( type *cl, void(type::*callback)() )
  {
    windowCloseEventCallback = [cl, callback](){ (cl->*callback)(); };
    glutCloseFunc( &window::windowCloseEventHandler );
    return 0;
  }

  void drawItems( const std::vector<std::unique_ptr<object>> &itemVector ) const;

  void mainLoop() const;
private:
  vector3D cameraPosition = { 0, 0, 50 };
  static void redrawEventHandler();
  static void idleEventHandler();
  static void timerEventHandler( int );
  static void resizeEventHandler( int, int );
  static void keyPressEventHandler( unsigned char, int, int );
  static void keyReleaseEventHandler( unsigned char, int, int );
  static void specialKeyPressEventHandler( int, int, int );
  static void specialKeyReleaseEventHandler( int, int, int );
  static void windowCloseEventHandler();

  int timerHandlerId = 0;

  void resizeWindowAction( int w, int h );

  std::function<void()> redrawEventCallback;
  std::function<void()> idleEventCallback;
  std::map< int, std::function<void()> > timerEventCallbacks;
  std::function<void( int, int )> resizeEventCallback;
  std::function<void( unsigned char, int, int )> keyPressEventCallback;
  std::function<void( unsigned char, int, int )> keyReleaseEventCallback;
  std::function<void( int, int, int )> specialKeyPressEventCallback;
  std::function<void( int, int, int )> specialKeyReleaseEventCallback;
  std::function<void()> windowCloseEventCallback;
};
