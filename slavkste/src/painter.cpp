#include "painter.hpp"
#include "object.hpp"
#include "complexObject.hpp"


using namespace std;

const double CPainter::minDrawLength = 20;


CPainter::CPainter( std::function<void()> callback )
  : redrawCallback( move( callback ) )
{}

double CPainter::start( int x, int y, vector<CPhysicsObject *> &objects )
{
  lastMousePosition = { (double)x, (double)y };

  TVector<2> accumulatedShift;

  size_t rayCount = 8;
  for( size_t idx = 0; idx < rayCount; ++idx )
  {
    auto dir =
            TVector<2>::canonical( 0 ).rotated( 2 * M_PI * (double)idx / (double)rayCount );
    for( auto object: objects )
    {
      double rayLength = object->rayTrace( lastMousePosition, dir );
      if( rayLength > drawWidth )
        continue;
      if( rayLength < drawWidth / 2 )
      {
        reset();
        return 0;
      }
      accumulatedShift -= dir * ( drawWidth - rayLength );
    }
  }

  lastMousePosition += accumulatedShift;

  currentlyDrawn = new CComplexObject( drawWidth );
  currentlyDrawn->addVertex( lastMousePosition );
  objects.push_back( currentlyDrawn );
  return minDrawLength / 2;
}

double CPainter::addPoint( int x, int y, vector<CPhysicsObject *> &objects )
{
  if( !currentlyDrawn )
    return start( x, y, objects );

  TVector<2> newMousePosition = { (double)x, (double)y };
  if( newMousePosition.distance( lastMousePosition ) < minDrawLength )
    return 0;

  TVector<2> direction = newMousePosition - lastMousePosition;
  TVector<2> normal = crossProduct( direction ).stretchedTo( drawWidth / 2 );

  for( auto &dir: { direction,
                    direction + 0.5 * normal,
                    direction - 0.5 * normal,
                    direction + 1.5 * normal,
                    direction - 1.5 * normal } )
  {
    double len = addPoint( dir, objects );
    if( len != 0 )
    {
      redrawCallback();
      return len;
    }
  }
  return 0;
}

double CPainter::stop( int x, int y, std::vector<CPhysicsObject *> &objects )
{
  if( !currentlyDrawn )
    return 0;
  double len = 0;
  if( lastMousePosition.distance( { (double)x, (double)y } ) <= minDrawLength / 2 )
  {
    len = addPoint( x, y, objects );
  }
  stop();
  return len;
}

void CPainter::stop()
{
  if( !currentlyDrawn )
    return;
  currentlyDrawn->spawn( density );
  reset();
}

void CPainter::reset()
{
  lastMousePosition = { NAN, NAN };
  currentlyDrawn = nullptr;
}

double CPainter::addPoint( const TVector<2> &direction, std::vector<CPhysicsObject *> &objects )
{
  TVector<2> normal = crossProduct( direction ).stretchedTo( drawWidth * 0.8 );

  double maxMoveLength = direction.norm();
  for( auto &startOffset: { normal + direction.stretchedTo( drawWidth ) * 0.2,
                            1.2 * direction.stretchedTo( drawWidth ),
                            -normal + direction.stretchedTo( drawWidth ) * 0.2 } )
  {
    for( const auto &object: objects )
    {
      if( object == currentlyDrawn )
        continue;
      double moveLength = object->rayTrace( lastMousePosition + startOffset,
                                            direction ) + drawWidth * 0.2;
      if( moveLength >= maxMoveLength )
        continue;
      maxMoveLength = moveLength;
      if( maxMoveLength < minDrawLength / 2 )
        return 0;
    }
  }

  lastMousePosition += direction.stretchedTo( maxMoveLength );

  currentlyDrawn->addVertex( lastMousePosition );

  return maxMoveLength;
}

