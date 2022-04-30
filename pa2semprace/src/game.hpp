#pragma once
#include "window.hpp"
#include "physicsEngine.hpp"
#include <vector>
#include <memory>

class game
{
public:
  game();
  void update();
  void nextFrame();
  void mainLoop();
  void gameEnd();
private:
  window m_window;
//  physicsEngine m_engine;

  std::vector<std::unique_ptr<object>> objects;
};
