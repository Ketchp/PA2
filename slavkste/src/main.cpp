#include "game.hpp"
#include <iostream>


int main( int argc, char **argv )
{
  try
  {
    CGame mainGame( &argc, argv );

    mainGame.mainLoop();
  }
  catch( const std::invalid_argument &e )
  {
    std::cerr << e.what();
  }

  std::cout << "Thanks for playing." << std::endl;
  return 0;
}
