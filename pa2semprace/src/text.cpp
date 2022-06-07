#include "text.hpp"

using namespace std;

CText::CText( int id, TVector<2> position, std::string text )
  : CObject( id, position ), text( move( text ) )
{}

void CText::render( CWindow &win ) const
{
  glTranslated( 0, 0, 1 );
  win.drawText( m_position, text );
  glTranslated( 0, 0, -1 );
}


CObject &CText::rotate( double angle )
{
  return *this;
}
