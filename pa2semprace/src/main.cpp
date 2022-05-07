#include "game.hpp"
#include <iostream>

int main(int argc, char **argv)
{
  CGame mainGame( &argc, argv );

  mainGame.mainLoop();
  std::cout << "Thanks for playing :)" << std::endl;
  return 0;
}
