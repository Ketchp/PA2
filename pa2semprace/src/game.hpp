#pragma once
#include "window.hpp"
#include "physicsEngine.hpp"
#include <vector>
#include <memory>
#include "jsonParser.hpp"
#include "helpers.hpp"
#include "cmath"

class CGame
{
public:
  CGame( int *argcPtr, char *argv[] );
  void mainLoop();
private:
  void loadLevel( const std::string &levelFileName );
  void loadLevelScreen( const jsonObject &screenDescription );
  static math::vector loadLevelSize( const jsonArray &levelSizeDescription );
  void loadItems( const jsonArray &itemsDescription );
  void loadItem( const jsonObject &itemDescription );
//  static int loadItemId( const jsonObject &itemDescription );
  static physicsAttributes loadItemPhysics( const jsonObject &itemDescription );
  static std::string loadItemType( const jsonObject &itemDescription );
  static math::vector loadItemPosition( const jsonObject &itemDescription );
  static alphaColor loadItemColor( const jsonObject &itemDescription );
  static double loadItemSize1D( const jsonObject &itemDescription );
  static math::vector loadItemSize2D( const jsonObject &itemDescription );

  void mouseClick( int button, int state, int x, int y );
  void mouseMove( int x, int y );

  void redraw();
  void nextFrame();


  math::vector lastMousePosition{ NAN, NAN };
  CWindow m_window;
  CPhysicsEngine m_engine;
  std::vector<std::unique_ptr<CObject>> m_physicsObjects;
  std::vector<std::unique_ptr<CObject>> m_controlObjects;
};
