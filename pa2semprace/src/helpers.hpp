#pragma once
#include <GL/freeglut.h>
#include <array>


struct vector2D
{
  GLfloat x, y;
  vector2D operator+( vector2D & );
  vector2D &operator+=( vector2D );
};

struct vector3D
{
  GLfloat x, y, z;
};

struct alphaColor
{
  GLclampf red, green, blue, alpha;
};