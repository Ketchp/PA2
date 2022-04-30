#include "object.hpp"

object::object( int id )
  :m_ID( id )
{}

void solidObject::applyColorFill() const
{
  glColor4f( m_color.red,
             m_color.green,
             m_color.blue,
             m_color.alpha );
}

rectangle::rectangle( int id, vector2D pos, GLfloat w, GLfloat h, alphaColor col )
  : solidObject( id, col ), m_position( pos ), m_width( w ), m_height( h )
{}

rectangle::rectangle( int id, GLfloat x, GLfloat y, GLfloat w, GLfloat h, alphaColor col )
  : rectangle( id, { x, y }, w, h, col )
{}

void rectangle::render() const
{
  applyColorFill();
  glBegin( GL_TRIANGLE_STRIP );

  glVertex2f( m_position.x, m_position.y );
  glVertex2f( m_position.x, m_position.y + m_height );
  glVertex2f( m_position.x + m_width, m_position.y );
  glVertex2f( m_position.x + m_width, m_position.y + m_height );

  glEnd();
}

solidObject::solidObject( int id, alphaColor col )
  : object( id ), m_color( col )
{}
