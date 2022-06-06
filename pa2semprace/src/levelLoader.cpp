#include "levelLoader.hpp"

using namespace std;
using collisionSubType = TCheck::subType::collisionType::collisionCondition;

TCheck::TCheck( ECheckType type, EActionType action, unsigned char button )
  : type( type ), action( action )
{
  spec.button = button;
}

TCheck::TCheck( ECheckType type, EActionType action, ETag tag1, ETag tag2 )
  : type( type ), action( action )
{
  spec.collisionType.collisionCond = collisionSubType::TagTag;
  spec.collisionType.first.tag = tag1;
  spec.collisionType.second.tag = tag2;
}

TCheck::TCheck( ECheckType type, EActionType action, ETag tag, int id )
  : type( type ), action( action )
{
  spec.collisionType.collisionCond = collisionSubType::TagId;
  spec.collisionType.first.tag = tag;
  spec.collisionType.second.id = id;

}

TCheck::TCheck( ECheckType type, EActionType action, int id1, int id2 )
  : type( type ), action( action )
{
  spec.collisionType.collisionCond = collisionSubType::IdId;
  spec.collisionType.first.id = id1;
  spec.collisionType.second.id = id2;
}


CLevelLoader::CLevelLoader( string levelFileName )
  : m_currentLevelFileName( move( levelFileName ) )
{}

void CLevelLoader::loadLevel( CWindow &levelWindow,
                              CPhysicsEngine &levelPhysicsEngine,
                              vector<CObject *> &levelObjects,
                              vector<TCheck> &levelChecks,
                              CPainter &painter,
                              EActionType action )
{
  m_levelWindow = &levelWindow;
  m_levelEngine = &levelPhysicsEngine;
  m_objects = &levelObjects;
  m_checks = &levelChecks;
  m_painter = &painter;
  for( auto object: *m_objects )
    delete object;
  m_objects->resize( 0 );

  if( action == EActionType::nextLevel )
    m_currentLevelFileName = m_nextLevelFileName;

  try
  {
    CJsonDocument json( m_currentLevelFileName );

    if( json.m_type != EJsonType::jsonObjectType )
      throw invalid_argument( "Level description must be json object.\n" );

    auto &levelDescription = json.get();

    if( levelDescription.count( "scene" ) )
      loadScene( levelDescription[ "scene" ].getObject() );

    if( levelDescription.count( "items" ) )
      loadItems( levelDescription[ "items" ].getArray() );

    if( levelDescription.count( "checks" ) )
      loadChecks( levelDescription[ "checks" ].getArray() );

  }
  catch( invalid_argument &e )
  {
    cerr << e.what();
    throw invalid_argument( "Error parsing level file." );
  }
}

void CLevelLoader::loadScene( const CJsonObject &sceneDescription )
{
  loadTitle( sceneDescription );
  loadSceneSize( sceneDescription );

  if( sceneDescription.count( "fields" ) )
    loadFields( sceneDescription[ "fields" ].getArray() );

  loadPenAttributes( sceneDescription );
  loadNextFileName( sceneDescription );
}

void CLevelLoader::loadTitle( const CJsonObject &sceneDescription )
{
  if( sceneDescription.count( "title" ) )
    m_levelWindow->changeTitle( sceneDescription[ "title" ].toString() );
  else
    m_levelWindow->changeTitle( "Gravity falls" );
}

void CLevelLoader::loadSceneSize( const CJsonObject &sceneDescription )
{
  if( !sceneDescription.count( "size" ) )
  {
    m_levelWindow->resizeView( 0, 800,
                               0, 600 );
    return;
  }

  auto sceneSize = sceneDescription[ "size" ].getArray();
  if( sceneSize.size() != 2 )
    throw invalid_argument( "Scene size must be array of size 2." );

  m_levelWindow->resizeView( 0, sceneSize[ 0 ].toDouble(),
                             0, sceneSize[ 1 ].toDouble() );
}

void CLevelLoader::loadFields( const CJsonArray &fieldsDescription )
{
  for( size_t idx = 0; idx < fieldsDescription.size(); ++idx )
    loadField( fieldsDescription[ idx ] );
}

void CLevelLoader::loadField( const CJsonValue &fieldDescription )
{
  if( fieldDescription.m_type == EJsonType::jsonStringType )
    if( fieldDescription.getJsonString().toString() == "gravity" )
      m_levelEngine->addField( CForceField::gravitationalField() );
}

void CLevelLoader::loadPenAttributes( const CJsonObject &sceneDescription )
{
  if( !sceneDescription.count( "pen" ) )
  {
    m_painter->drawWidth = 8;
    m_painter->density = 50;
    return;
  }
  const auto &penDescription = sceneDescription[ "pen" ].getObject();
  if( penDescription.count( "density" ) )
    m_painter->density = penDescription[ "density" ].toDouble();
  else
    m_painter->density = 50;

  if( penDescription.count( "width" ) )
    m_painter->drawWidth = penDescription[ "width" ].toDouble();
  else
    m_painter->drawWidth = 8;
}

void CLevelLoader::loadNextFileName( const CJsonObject &sceneDescription )
{
  m_nextLevelFileName = "";
  if( sceneDescription.count( "next" ) )
    m_nextLevelFileName = sceneDescription[ "next" ].toString();
}


void CLevelLoader::loadItems( const CJsonArray &itemsDescription )
{
  for( size_t idx = 0; idx < itemsDescription.size(); ++idx )
    loadItem( itemsDescription[ idx ].getObject() );
}

void CLevelLoader::loadItem( const CJsonObject &itemDescription )
{
  int itemID = loadItemId( itemDescription );

  string itemType = itemDescription[ "type" ].toString();

  double density = loadDensity( itemDescription );
  ETag tags = loadTags( itemDescription );

  if( itemType == "circle" )
    loadCircle( itemDescription, itemID, density, tags );
  else if( itemType == "rectangle" )
    loadRectangle( itemDescription, itemID, density, tags );
}

int CLevelLoader::loadItemId( const CJsonObject &itemDescription )
{
  if( itemDescription.count( "id" ) )
    return itemDescription[ "id" ].toInt();
  return idPool--;
}

ETag CLevelLoader::loadTags( const CJsonObject &itemDescription )
{
  if( itemDescription.count( "tag" ) )
    return loadTag( itemDescription[ "tag" ].toString());
  if( itemDescription.count( "tags" ) )
  {
    ETag tag( NONE );
    const auto &tags = itemDescription[ "tags" ].getArray();
    for( size_t idx = 0; idx < tags.size(); ++idx )
      tag |= loadTag( tags[ idx ].toString() );
    return tag;
  }
  return NONE;
}

ETag CLevelLoader::loadTag( const std::string &tag )
{
  if( tag == "win zone" )
    return WIN_ZONE;
  else if( tag == "no draw zone" )
    return NO_DRAW_ZONE;
  else
    return NONE;
}


TVector<2> CLevelLoader::loadVector2D( const CJsonArray &jsonArray )
{
  return { jsonArray[ 0 ].toDouble(), jsonArray[ 1 ].toDouble() };
}

void CLevelLoader::loadCircle( const CJsonObject &circleDescription,
                               int id, double density, ETag tags )
{
  double radius = circleDescription[ "size" ].toDouble();
  TVector<2> position = loadVector2D( circleDescription[ "position" ].getArray() );
  CObject *newObj = new CCircle( id, position, radius, density );
  newObj->addTag( tags );
  m_objects->push_back( newObj );

}

void CLevelLoader::loadRectangle( const CJsonObject &rectDescription,
                                  int id, double density, ETag tags )
{
  TVector<2> size = loadVector2D( rectDescription[ "size" ].getArray() );
  TVector<2> position = loadVector2D( rectDescription[ "position" ].getArray() );
  double rotation = loadRotation( rectDescription );
  CObject *newObj = new CRectangle( id, position,
                                    size[ 0 ], size[ 1 ],
                                    rotation, density );
  newObj->addTag( tags );
  m_objects->push_back( newObj );
}

double CLevelLoader::loadDensity( const CJsonObject &itemDescription )
{
  if( itemDescription.count( "physics" ) && itemDescription[ "physics" ].count( "density" ) )
    return itemDescription[ "physics" ][ "density" ].toDouble();
  return HUGE_VAL;
}

double CLevelLoader::loadRotation( const CJsonObject &itemDescription )
{
  if( itemDescription.count( "rotation" ) )
    return itemDescription[ "rotation" ].toDouble();
  return 0;
}

void CLevelLoader::loadChecks( const CJsonArray &checksDescription )
{
  for( size_t idx = 0; idx < checksDescription.size(); ++idx )
    loadCheck( checksDescription[ idx ].getObject() );
}

void CLevelLoader::loadCheck( const CJsonObject &checkDescription )
{
  if( !checkDescription.count( "checkType" ) )
    throw invalid_argument( "Check must contain checkType field." );

  string checkType = checkDescription[ "checkType" ].toString();
  if( checkType == "key press" )
    loadCheckPress( checkDescription );
  else if( checkType == "collision" )
    loadCheckCollision( checkDescription );
  else
    cerr << "Warning: Unknown check type... skipping" << endl;
}

EActionType CLevelLoader::loadCheckAction( const CJsonObject &checkDescription )
{
  if( !checkDescription.count( "action" ) )
    return EActionType::nextLevel;
  string action = checkDescription[ "action" ].toString();
  if( action == "reset" )
    return EActionType::resetLevel;
  else if( action == "next" )
    return EActionType::nextLevel;
  throw invalid_argument( "Unknown action type \"" + action + "\"" );
}

void CLevelLoader::loadCheckPress( const CJsonObject &checkDescription )
{
  unsigned char key = checkDescription[ "key" ].toString()[ 0 ];
  EActionType action = loadCheckAction( checkDescription );

  m_checks->push_back( { ECheckType::keyPress, action, key } );
}

void CLevelLoader::loadCheckCollision( const CJsonObject &checkDescription )
{
  collisionSubType checkCollisionType = loadCheckCollisionType( checkDescription );
  switch( checkCollisionType )
  {
  case collisionSubType::TagTag:
    loadCheckTagTag( checkDescription );
    break;
  case collisionSubType::TagId:
    loadCheckTagId( checkDescription );
    break;
  case collisionSubType::IdId:
    loadCheckIdId( checkDescription );
  }
}

collisionSubType
CLevelLoader::loadCheckCollisionType( const CJsonObject &checkDescription )
{
  if( !checkDescription.count( "collisionType" ) )
    return collisionSubType::TagTag;
  string type = checkDescription[ "collisionType" ].toString();
  if( type == "IdId" )
    return collisionSubType::IdId;
  if( type == "TagId" )
    return collisionSubType::TagId;
  if( type == "TagTag" )
    return collisionSubType::TagTag;
  throw invalid_argument( "Unknown collision check subtype." );
}

void CLevelLoader::loadCheckTagTag( const CJsonObject &checkDescription )
{
  // todo
}

void CLevelLoader::loadCheckTagId( const CJsonObject &checkDescription )
{
  int id = loadCheckId( checkDescription );
  EActionType action = loadCheckAction( checkDescription );

  m_checks->push_back( { ECheckType::collision, action, ETag::WIN_ZONE, id } );
}

int CLevelLoader::loadCheckId( const CJsonObject &checkDescription )
{
  if( !checkDescription.count( "id" ) )
    return 0;
  return checkDescription[ "id" ].toInt();
}

void CLevelLoader::loadCheckIdId( const CJsonObject &checkDescription )
{
  // todo
}


