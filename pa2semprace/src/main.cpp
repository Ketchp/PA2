#include <GL/freeglut.h>
#include <iostream>
#include "game.hpp"


int main(int argc, char **argv)
{
  // init GLUT and create window
  glutInit(&argc, argv);
  game mainGame;

  mainGame.mainLoop();
  return 0;
}
