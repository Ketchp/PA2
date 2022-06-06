#include "painter.hpp"
#include "object.hpp"
#include "complexObject.hpp"

using namespace std;

const double CPainter::minDrawLength = 20;


CPainter::CPainter( std::function<void()> callback )
  : redrawCallback( move( callback ) )
{}

void CPainter::start( int x, int y, vector<CObject *> &objects )
{
  lastMousePosition = { (double)x, (double)y };
  currentlyDrawn = new CComplexObject( 0, drawWidth );
  currentlyDrawn->addVertex( lastMousePosition );
  for( auto object: objects )
    if( currentlyDrawn->getManifold( object ) )
    {
      delete currentlyDrawn;
      currentlyDrawn = nullptr;
      lastMousePosition = { NAN, NAN };
      return;
    }
  objects.push_back( currentlyDrawn );
}

void CPainter::addPoint( int x, int y, const vector<CObject *> &objects )
{
  TVector<2> newMousePosition = { (double)x, (double)y };
  TVector<2> direction = newMousePosition - lastMousePosition;
  TVector<2> normal = crossProduct( direction ).stretchedTo( currentlyDrawn->m_width );

  double maxMoveLength = direction.norm();
  for( auto &startOffset: { normal,
                            direction.stretchedTo( currentlyDrawn->m_width ),
                            -normal } )
  {
    for( const auto &object: objects )
    {
      if( object == currentlyDrawn )
        continue;
      double moveLength = object->rayTrace( lastMousePosition + startOffset,
                                            direction );
      if( moveLength >= maxMoveLength )
        continue;
      maxMoveLength = moveLength;
      if( maxMoveLength < minDrawLength / 2 )
        return;
    }
  }

  lastMousePosition += direction.stretchedTo( maxMoveLength );

  currentlyDrawn->addVertex( lastMousePosition );

  redrawCallback();
}

void CPainter::stop( int x, int y, const std::vector<CObject *> &objects )
{
  if( lastMousePosition.distance( { (double)x, (double)y } ) > minDrawLength / 2 )
    addPoint( x, y, objects );
  lastMousePosition = { NAN, NAN };
  currentlyDrawn = nullptr;
}

