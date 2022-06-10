#include "painter.hpp"
#include "object.hpp"
#include "complexObject.hpp"

using namespace std;

const double CPainter::minDrawLength = 20;


CPainter::CPainter( std::function<void()> callback )
  : redrawCallback( move( callback ) )
{}

void CPainter::start( int x, int y, vector<CPhysicsObject *> &objects )
{
  lastMousePosition = { (double)x, (double)y };

  size_t rayCount = 8;
  for( size_t idx = 0; idx < rayCount; ++idx )
  {
    auto dir =
            TVector<2>::canonical( 0 ).rotated( 2 * M_PI * (double)idx / (double)rayCount );
    for( auto object: objects )
    {
      if( object->rayTrace( lastMousePosition, dir ) > drawWidth )
        continue;
      reset();
      return;
    }
  }

  currentlyDrawn = new CComplexObject( drawWidth );
  currentlyDrawn->addVertex( lastMousePosition );
  objects.push_back( currentlyDrawn );
}

void CPainter::addPoint( int x, int y, const vector<CPhysicsObject *> &objects )
{
  if( !currentlyDrawn )
    return;
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

void CPainter::stop( int x, int y, const std::vector<CPhysicsObject *> &objects )
{
  if( !currentlyDrawn )
    return;
  if( lastMousePosition.distance( { (double)x, (double)y } ) > minDrawLength / 2 )
    addPoint( x, y, objects );
  currentlyDrawn->spawn( density );
  reset();
}

void CPainter::reset()
{
  lastMousePosition = { NAN, NAN };
  currentlyDrawn = nullptr;
}

