#include <GL/freeglut.h>
#include "linearAlgebra.hpp"
#include <iostream>

int main( int argc, char *argv[] )
{
//  glutInit( &argc, argv );
//  glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
//  glutInitWindowPosition( 100, 100 );
//  glutInitWindowSize( 320, 320 );
//  /*int winID = //*/
//  glutCreateWindow( "test" );
//  // glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );
//
//  glEnable( GL_DEPTH_TEST );
//
//  glutMainLoop();
//
  // glutDestroyWindow( winID );

  math::vector a(5, 6);
  std::cout << a[0] << a[1] << std::endl;

}