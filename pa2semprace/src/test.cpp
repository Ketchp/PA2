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

  TVector<2> a{5, 6};

  TMatrix<2, 2> b{ {8, 7}, {6, 4} };

  TVector<2> c = b * a;

  TVector<2> d = 2.0 * a;


  std::cout << a[0] << a[1] << std::endl;

  std::cout << c[0] << c[1] << std::endl;
  std::cout << d[0] << d[1] << std::endl;
}