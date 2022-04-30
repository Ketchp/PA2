#include "helpers.hpp"

vector2D vector2D::operator+( vector2D &rhs )
{
  return { x + rhs.x, y + rhs.y };
}

vector2D &vector2D::operator+=( vector2D rhs )
{
  return *this = *this + rhs;
}
