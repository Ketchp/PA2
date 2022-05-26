#pragma once
#include "window.hpp"
#include "physicsEngine.hpp"
#include "levelLoader.hpp"
#include "jsonParser.hpp"
#include "helpers.hpp"
#include "painter.hpp"
#include "object.hpp"
#include <cmath>
#include <vector>
#include <memory>

class CGame
{
public:
  CGame( int *argcPtr, char *argv[] );
  ~CGame();
  void mainLoop();
private:
  void keyPress( unsigned char, int, int );
  void redraw();
  void nextFrame();

  bool m_paused = true;
  const int framerate = 100;

  CWindow m_window;
  CLevelLoader m_levelLoader;
  CPhysicsEngine m_engine;
  std::vector<CObject *> m_objects;
  std::vector<TCheck> m_checks;
  CPainter m_painter;
};
