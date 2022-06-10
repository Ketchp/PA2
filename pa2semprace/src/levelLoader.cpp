#include "levelLoader.hpp"

using namespace std;

CLevelLoader::CLevelLoader( CWindow &window,
                            CPhysicsEngine &engine,
                            std::vector<CPhysicsObject *> &objects,
                            std::vector<CText *> &texts,
                            CPainter &painter,
                            string firstLevelName )
  : m_currentLevelFileName( move( firstLevelName ) ),
    m_window( window ),
    m_engine( engine ),
    m_objects( objects ),
    m_texts( texts ),
    m_painter( painter )
{}

void CLevelLoader::loadLevel( EActionType action )
{
  m_engine.reset();

  for( auto object: m_objects )
    delete object;
  m_objects.clear();

  for( auto text: m_texts )
    delete text;
  m_texts.clear();

  m_painter.reset();

  if( action == EActionType::nextLevel )
    m_currentLevelFileName = m_nextLevelFileName;

  try
  {
    CJsonDocument json( m_currentLevelFileName );
    cout << "Loading file:" << m_currentLevelFileName << endl;

    if( json.m_type != EJsonType::jsonObjectType )
      throw invalid_argument( "Level description must be json object.\n" );

    auto &levelDescription = json.get();

    if( levelDescription.count( "scene" ) )
      loadScene( levelDescription[ "scene" ].getObject() );

    if( levelDescription.count( "items" ) )
      loadItems( levelDescription[ "items" ].getArray() );

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
    m_window.changeTitle( sceneDescription[ "title" ].toString() );
  else
    m_window.changeTitle( "Gravity falls" );
}

void CLevelLoader::loadSceneSize( const CJsonObject &sceneDescription )
{
  if( !sceneDescription.count( "size" ) )
  {
    m_window.resizeView( 0, 800,
                         0, 600 );
    return;
  }

  auto sceneSize = sceneDescription[ "size" ].getArray();
  if( sceneSize.size() != 2 )
    throw invalid_argument( "Scene size must be array of size 2." );

  m_window.resizeView( 0, sceneSize[ 0 ].toDouble(),
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
      m_engine.addField( CForceField::gravitationalField() );
}

void CLevelLoader::loadPenAttributes( const CJsonObject &sceneDescription )
{
  m_painter.drawWidth = 8;
  m_painter.density = 50;
  if( !sceneDescription.count( "pen" ) )
    return;

  const auto &penDescription = sceneDescription[ "pen" ].getObject();
  if( penDescription.count( "density" ) )
    m_painter.density = penDescription[ "density" ].toDouble();

  if( penDescription.count( "width" ) )
    m_painter.drawWidth = penDescription[ "width" ].toDouble();
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
  string itemType = itemDescription[ "type" ].toString();

  ETag tags = loadTags( itemDescription );

  if( itemType == "circle" )
    loadCircle( itemDescription, tags );
  else if( itemType == "rectangle" )
    loadRectangle( itemDescription, tags );
  else if( itemType == "text" )
    loadText( itemDescription, tags );
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
  if( tag == "transparent" )
    return TRANSPARENT;
  if( tag == "non solid" )
    return NON_SOLID;
  if( tag == "target" )
    return TARGET;
  return NONE;
}


TVector<2> CLevelLoader::loadVector2D( const CJsonArray &jsonArray )
{
  return { jsonArray[ 0 ].toDouble(), jsonArray[ 1 ].toDouble() };
}

void CLevelLoader::loadCircle( const CJsonObject &circleDescription,
                               ETag tags )
{
  double radius = circleDescription[ "size" ].toDouble();
  double density = loadDensity( circleDescription );
  TVector<2> position = loadVector2D( circleDescription[ "position" ].getArray() );
  CPhysicsObject *newObj = new CCircle( position, radius, density );
  newObj->addTag( tags );
  m_objects.push_back( newObj );

}

void CLevelLoader::loadRectangle( const CJsonObject &rectDescription,
                                  ETag tags )
{
  TVector<2> position = loadVector2D( rectDescription[ "position" ].getArray() );
  TVector<2> size = loadVector2D( rectDescription[ "size" ].getArray() );
  double rotation = loadRotation( rectDescription );
  double density = loadDensity( rectDescription );
  CPhysicsObject *newObj = new CRectangle( position, size,
                                           rotation, density );
  newObj->addTag( tags );
  m_objects.push_back( newObj );
}

void CLevelLoader::loadText( const CJsonObject &textDescription, ETag tags )
{
  TVector<2> position = loadVector2D( textDescription[ "position" ].getArray() );
  string text = textDescription[ "text" ].toString();
  auto *newObj = new CText( position, text );
  newObj->addTag( tags );
  m_texts.push_back( newObj );
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
