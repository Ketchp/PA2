//#include <GL/glut.h>
//#include <cmath>
//#include <iostream>
//
//// angle of rotation for the camera direction
//float angle=0.0;
//// actual vector representing the camera's direction
//float lx=0.0f,lz=-1.0f;
//// XZ position of the camera
//float x=0.0f,z=5.0f;
//
//void changeSize(int w, int h) {
//
//  // Prevent a divide by zero, when window is too short
//  // (you cant make a window of zero width).
//  if(h == 0)
//    h = 1;
//  float ratio = 1.0* w / h;
//
//  // Use the Projection Matrix
//  glMatrixMode(GL_PROJECTION);
//
//  // Reset Matrix
//  glLoadIdentity();
//
//  // Set the viewport to be the entire window
//  glViewport(0, 0, w, h);
//
//  // Set the correct perspective.
//  gluPerspective(45,ratio,1,1000);
//
//  // Get Back to the Modelview
//  glMatrixMode(GL_MODELVIEW);
//}
//
//void drawSnowMan() {
//
//  glColor3f(1.0f, 1.0f, 1.0f);
//
//// Draw Body
//  glTranslatef(0.0f ,0.75f, 0.0f);
//  glutSolidSphere(0.75f,20,20);
//
//// Draw Head
//  glTranslatef(0.0f, 1.0f, 0.0f);
//  glutSolidSphere(0.25f,20,20);
//
//// Draw Eyes
//  glPushMatrix();
//  glColor3f(0.0f,0.0f,0.0f);
//  glTranslatef(0.05f, 0.10f, 0.18f);
//  glutSolidSphere(0.05f,10,10);
//  glTranslatef(-0.1f, 0.0f, 0.0f);
//  glutSolidSphere(0.05f,10,10);
//  glPopMatrix();
//
//// Draw Nose
//  glColor3f(1.0f, 0.5f , 0.5f);
//  glutSolidCone(0.08f,0.5f,10,2);
//}
//
//void renderScene(void) {
//
//  // Clear Color and Depth Buffers
//
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//  // Reset transformations
//  glLoadIdentity();
//  // Set the camera
//  gluLookAt(	x, 1.0f, z,
//              x+lx, 1.0f,  z+lz,
//              0.0f, 1.0f,  0.0f);
//
//  // Draw ground
//  glColor3f(0.9f, 0.9f, 0.9f);
//  glBegin(GL_QUADS);
//  glVertex3f(-100.0f, 0.0f, -100.0f);
//  glVertex3f(-100.0f, 0.0f,  100.0f);
//  glVertex3f( 100.0f, 0.0f,  100.0f);
//  glVertex3f( 100.0f, 0.0f, -100.0f);
//  glEnd();
//
//  // Draw 36 SnowMen
//  for(int i = -3; i < 3; i++)
//    for(int j=-3; j < 3; j++) {
//      glPushMatrix();
//      glTranslatef(i*10.0,0,j * 10.0);
//      drawSnowMan();
//      glPopMatrix();
//    }
//
//  glutSwapBuffers();
//}
//
//void processSpecialKeys(int key, int xx, int yy) {
//
//  float fraction = 0.1f;
//
//  switch (key) {
//  case GLUT_KEY_LEFT :
//    angle -= 0.01f;
//    lx = sin(angle);
//    lz = -cos(angle);
//    break;
//  case GLUT_KEY_RIGHT :
//    angle += 0.01f;
//    lx = sin(angle);
//    lz = -cos(angle);
//    break;
//  case GLUT_KEY_UP :
//    x += lx * fraction;
//    z += lz * fraction;
//    break;
//  case GLUT_KEY_DOWN :
//    x -= lx * fraction;
//    z -= lz * fraction;
//    break;
//  }
//}
//
//void processNormalKeys(unsigned char key, int x, int y) {
//
//  if (key == 27)
//    exit(0);
//}
//
//int main(int argc, char **argv) {
//
//  // init GLUT and create window
//
//  glutInit(&argc, argv);
//
//
//  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
//
//
//
//  glutInitWindowPosition(100,100);
//  glutInitWindowSize(320,320);
//
//#ifdef __DEBUG
//  std::printf( "%s\n", glGetString( GL_VERSION ) );
//#endif
//
//  glutCreateWindow("Lighthouse3D - GLUT Tutorial");
//
//
//
//  // register callbacks
//  glutDisplayFunc(renderScene);
//  glutReshapeFunc(changeSize);
//  glutIdleFunc(renderScene);
//  glutKeyboardFunc(processNormalKeys);
//  glutSpecialFunc(processSpecialKeys);
//
//  // OpenGL init
//  glEnable(GL_DEPTH_TEST);
//
//
//  // enter GLUT event processing cycle
//  glutMainLoop();
//
//  return 1;
//}

#include <GL/freeglut.h>    // library cross-platform toolkit windows and managing input operations

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

/**
* analyse the version
*/
string makeMeString(GLint versionRaw) {
  stringstream ss;
  string str = "\0";

  ss << versionRaw;    // transfers versionRaw value into "ss"
  str = ss.str();        // sets the "str" string as the "ss" value
  return str;
}

/**
* Format the string as expected
*/
void formatMe(string *text) {
  string dot = ".";

  text->insert(1, dot); // transforms 30000 into 3.0000
  text->insert(4, dot); // transforms 3.0000 into 3.00.00
}

/**
* Message
*/
void consoleMessage() {
  const char *versionGL = "\0";
  GLint versionFreeGlutInt = 0;

  versionGL = (char *)(glGetString(GL_VERSION));
  versionFreeGlutInt = (glutGet(GLUT_VERSION));

  string versionFreeGlutString = makeMeString(versionFreeGlutInt);
  formatMe(&versionFreeGlutString);

  cout << endl;
  cout << "OpenGL version: " << versionGL << endl << endl;
  cout << "FreeGLUT version: " << versionFreeGlutString << endl << endl;
}

/**
* Manage display (to be implemented)
*/
void managerDisplay(void)
{
  glClear(GL_COLOR_BUFFER_BIT);                // clear the screen
  glutSwapBuffers();
}


/**
* Initialize FREEGLUT
*/
void initFreeGlut(int ac, char *av[]) {
  // A. init
  glutInit(&ac, av);                                // 1. inits glut with arguments from the shell
  glutInitDisplayString("");                        // 2a. sets display parameters with a string (obsolete)
  glutInitDisplayMode(GLUT_SINGLE);                // 2b. sets display parameters with defines
  glutInitWindowSize(600, 600);                    // 3. window size
  glutInitContextVersion(3, 3);                    // 4. sets the version 3.3 as current version (so some functions of 1.x and 2.x could not work properly)
  glutInitContextProfile(GLUT_CORE_PROFILE);        // 5. sets the version 3.3 for the profile core
  glutInitWindowPosition(500, 500);                // 6. distance from the top-left screen

  // B. create window
//  glutCreateWindow("BadproG - Hello world :D");    // 7. message displayed on top bar window

}

/**
* Manage keyboard
*/
void managerKeyboard(unsigned char key, int x, int y) {
  if (key == 27) { // 27 = ESC key
    exit(0);
  }
}

void test( void(*)(int, int, void*),void* )
{

}

/**
* Main, what else?
*/
int main(int argc, char** argv) {
  initFreeGlut(argc, argv);    // inits freeglut
  consoleMessage();            // displays message on the console

  // C. register the display callback function
  glutDisplayFunc(managerDisplay);                        // 8. callback function
  glutKeyboardFunc(managerKeyboard);
  glutKeyboardUpFuncUcall(test, nullptr);

  // D. main loop
  glutMainLoop();                                    // 9. infinite loop
  return 0;
}