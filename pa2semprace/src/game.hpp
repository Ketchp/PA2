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
  CGame( const CGame & ) = delete;
  CGame( CGame && ) = delete;
  CGame &operator=( const CGame & ) = delete;
  CGame &operator=( CGame && ) = delete;

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

  static long getMilliseconds();

  void setTimer();

  [[nodiscard]] bool checkPlayerHealth() const;

  [[nodiscard]] bool playerOnScreen();

  void drawPenalty( double );

  bool m_paused = true;
  bool pressed = false;
  const double frameLength = 40; //ms
  long lastFrame = 0;

  CWindow m_window;
  CPhysicsEngine m_engine;
  std::vector<CPhysicsObject *> m_objects;
  std::vector<CText *> m_text;
  CPainter m_painter;
  CLevelLoader m_levelLoader;
};
