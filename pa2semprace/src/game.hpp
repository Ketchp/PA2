#pragma once

#include "window.hpp"
#include "physicsEngine.hpp"
#include "levelLoader.hpp"
#include "jsonParser.hpp"
#include "painter.hpp"
#include "object.hpp"
#include <cmath>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>


class CGame
{
public:
  CGame( int *argcPtr, char *argv[] );

  ~CGame();

  void mainLoop();

private:
  void keyPress( unsigned char, int, int );

  void redraw();

  void drawHealthBar();

  void nextFrame();

  void clickHandler( int button, int state, int x, int y );

  void moveHandler( int x, int y );

  void start();

  void pause();

  static bool checkCollisions( const std::vector<TManifold> & );

  static bool checkCollision( const TManifold & );

  bool checkPlayerHealth() const;

  bool m_paused = true;
  bool m_finished = false;
  const double frameLength = 40; //ms
  long lastFrame = 0;

  CWindow m_window;
  CPhysicsEngine m_engine;
  std::vector<CPhysicsObject *> m_objects;
  std::vector<CText *> m_text;
  CPainter m_painter;
  CLevelLoader m_levelLoader;
};

