#include "levelLoader.hpp"
#include "jsonParser.hpp"

using namespace std;

CLevelLoader::CLevelLoader( string levelFileName )
  : m_levelDescriptionFile( move( levelFileName ) )
{}

void CLevelLoader::loadLevel( CWindow &levelWindow,
                              CPhysicsEngine &levelPhysicsEngine,
                              vector<CObject *> &levelObjects,
                              vector<TCheck> &levelChecks )
{
  m_levelWindow = &levelWindow;
  m_levelEngine = &levelPhysicsEngine;
  m_objects = &levelObjects;
  m_checks = &levelChecks;
  CJsonDocument json( m_levelDescriptionFile );
  if( json.m_type != EJsonType::jsonObjectType )
    throw invalid_argument( "Level description must be json object.\n" );

  auto &levelDescription = json.get();

  if( levelDescription.count( "scene" ) )
    loadScene( levelDescription[ "screen" ].getObject() );

  if( levelDescription.count( "items" ) )
    loadItems( levelDescription[ "items" ].getArray() );

  if( levelDescription.count( "controls" ) )
    loadControls( levelDescription[ "controls" ].getArray() );

  if( levelDescription.count( "checks" ) )
    loadChecks( levelDescription[ "checks" ].getArray() );
}

void CLevelLoader::loadScene( const CJsonObject &sceneDescription )
{
  loadSceneSize( sceneDescription );


  // todo load color
  // todo load gravity etc..
}

void CLevelLoader::loadSceneSize( const CJsonObject &sceneDescription )
{
  if( !sceneDescription.count( "size" ) )
  {
    m_levelWindow->resizeView( 0, 800,
                               0, 600 );
    return;
  }

  auto sceneSize = sceneDescription.getArray();
  if( sceneSize.size() != 2 )
    throw invalid_argument( "Scene size must be array of size 2." );

  m_levelWindow->resizeView( 0, sceneSize[ 0 ].toDouble(),
                             0, sceneSize[ 1 ].toDouble() );
}


void CLevelLoader::loadItems( const CJsonArray &itemsDescription )
{
  for( size_t idx = 0; idx < itemsDescription.size(); ++idx )
    loadItem( itemsDescription[ idx ].getObject() );
}

void CLevelLoader::loadItem( const CJsonObject &itemDescription )
{
  int itemID = idPool;
  if( itemDescription.count( "id" ) )
    itemID = itemDescription[ "id" ].toInt();
  else
    ++idPool;
  string itemType = itemDescription[ "type" ].toString();

  double density = loadDensity( itemDescription );

  if( itemType == "line" )
    loadLine( itemDescription, itemID, density );
  else if( itemType == "circle" )
    loadCircle( itemDescription, itemID, density );
  else if( itemType == "rectangle" )
    loadRectangle( itemDescription, itemID, density );
}

TVector<2> CLevelLoader::loadVector2D( const CJsonArray &jsonArray )
{
  return { jsonArray[ 0 ].toDouble(), jsonArray[ 1 ].toDouble() };
}

void CLevelLoader::loadLine( const CJsonObject &lineDescription, int id, double density )
{
  TVector<2> begin = loadVector2D( lineDescription[ "position" ][ 0 ].getArray() );
  TVector<2> end = loadVector2D( lineDescription[ "position" ][ 1 ].getArray() );
  m_objects->push_back( (CObject *)new CLine( id, begin, end, density ) );
}

void CLevelLoader::loadCircle( const CJsonObject &circleDescription, int id, double density )
{
  double radius = circleDescription[ "size" ].toDouble();
  TVector<2> position = loadVector2D( circleDescription[ "position" ].getArray() );
  m_objects->push_back( (CObject *)new CCircle( id, position, radius, density ) );
}

void CLevelLoader::loadRectangle( const CJsonObject &rectDescription, int id, double density )
{
  TVector<2> size = loadVector2D( rectDescription[ "size" ].getArray() );
  TVector<2> position = loadVector2D( rectDescription[ "position" ].getArray() );
  m_objects->push_back( (CObject *)CComplexObject::makeRectangle( id, position, size, density ) );
}

double CLevelLoader::loadDensity( const CJsonObject &itemDescription )
{
  if( itemDescription.count( "physics" ) && itemDescription[ "physics" ].count( "density" ) )
    return itemDescription[ "physics" ][ "density" ].toDouble();
  return HUGE_VAL;
}

void CLevelLoader::loadControls( const CJsonArray & )
{
  // todo
}

void CLevelLoader::loadChecks( const CJsonArray & )
{
 //todo
}
