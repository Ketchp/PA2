#include "game.hpp"
#include <iostream>
#include <chrono>
#include <thread>


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
  //std::cout << "*** Stack smashing detected ***: ./slavkste/main terminated\n"
  //             "Aborted (core dumped)" << std::endl;

  //std::this_thread::sleep_for(std::chrono::milliseconds( 5000 ));
  //std::cout << "Just kidding :)" << std::endl;
  std::cout << "Thanks for playing." << std::endl;
  return 0;
}
