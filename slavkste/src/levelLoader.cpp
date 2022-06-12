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
          m_painter( painter ){}

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

  hasPlayer = false;

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

    if( m_objects.empty() )
      throw invalid_argument( "Level must contain some objects.\n" );

    for( auto &obj: m_objects )
      if( obj->m_tag & ETag::PLAYER )
        swap( obj, m_objects[0] );
  }
  catch( invalid_argument &e )
  {
    cerr << e.what();
    throw invalid_argument( "Error parsing level file.\n" );
  }
}

void CLevelLoader::loadScene( const CJsonObject &sceneDescription )
{
  loadTitle( sceneDescription );
  loadSceneSize( sceneDescription );

  healthBar = true;
  if( sceneDescription.count( "bar" ) )
    healthBar = (bool)sceneDescription[ "bar" ];

  try
  {
    if( sceneDescription.count( "fields" ) )
      loadFields( sceneDescription[ "fields" ].getArray() );
  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Fields field must be an array.\n" );
  }

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

  try
  {
    const auto &sceneSize = sceneDescription[ "size" ].getArray();
    if( sceneSize.size() != 2 )
      throw invalid_argument( "Scene size must be array of size 2.\n" );

    double width = sceneSize[ 0 ].toDouble(),
           height = sceneSize[ 1 ].toDouble();

    if( width <= 0 || height <= 0 )
      throw invalid_argument( "Scene must have positive size.\n" );

    m_window.resizeView( 0, width,
                         0, height );
  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Scene size must be an array.\n" );
  }
}

void CLevelLoader::loadFields( const CJsonArray &fieldsDescription )
{
  for( size_t idx = 0; idx < fieldsDescription.size(); ++idx )
    loadField( fieldsDescription[ idx ] );
}

void CLevelLoader::loadField( const CJsonValue &fieldDescription )
{
  if( fieldDescription.m_type == EJsonType::jsonStringType )
    if( fieldDescription.toString() == "gravity" )
      m_engine.addField( CForceField::gravitationalField() );
}

void CLevelLoader::loadPenAttributes( const CJsonObject &sceneDescription )
{
  m_painter.drawWidth = 8;
  m_painter.density = 50;
  if( !sceneDescription.count( "pen" ) )
    return;

  try
  {
    const auto &penDescription = sceneDescription[ "pen" ].getObject();
    if( penDescription.count( "density" ) )
      m_painter.density = penDescription[ "density" ].toDouble();

    if( penDescription.count( "width" ) )
      m_painter.drawWidth = penDescription[ "width" ].toDouble();

    if( m_painter.density <= 0 )
      throw invalid_argument( "Draw density must be positive.\n" );

    if( m_painter.drawWidth <= 0 )
      throw invalid_argument( "Draw width must be positive.\n" );

  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Pen description must be an object.\n" );
  }
}

void CLevelLoader::loadNextFileName( const CJsonObject &sceneDescription )
{
  m_nextLevelFileName = "";
  if( sceneDescription.count( "next" ) )
    m_nextLevelFileName = sceneDescription[ "next" ].toString();
}


void CLevelLoader::loadItems( const CJsonArray &itemsDescription )
{
  try
  {
    for( size_t idx = 0; idx < itemsDescription.size(); ++idx )
      loadItem( itemsDescription[ idx ].getObject() );
  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Item description must be an object.\n" );
  }
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
    return loadTag( itemDescription[ "tag" ].toString() );
  if( itemDescription.count( "tags" ) )
  {
    ETag tag( NONE );
    try
    {
      const auto &tags = itemDescription[ "tags" ].getArray();
      for( size_t idx = 0; idx < tags.size(); ++idx )
        tag |= loadTag( tags[ idx ].toString() );
      return tag;
    }
    catch( const bad_cast & )
    {
      throw invalid_argument( "Tags must be an array.\n" );
    }
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
  if( tag == "player" )
  {
    if( hasPlayer )
      throw invalid_argument( "Level can contain only one player.\n" );
    hasPlayer = true;
    return PLAYER;
  }
  return NONE;
}


TVector<2> CLevelLoader::loadVector2D( const CJsonArray &jsonArray )
{
  try
  {
    return { jsonArray[ 0 ].toDouble(), jsonArray[ 1 ].toDouble() };
  }
  catch( const out_of_range & )
  {
    throw invalid_argument( "Expected array with at least 2 reals.\n" );
  }
}

void CLevelLoader::loadCircle( const CJsonObject &circleDescription,
                               ETag tags )
{
  try
  {
    double radius = circleDescription[ "size" ].toDouble();
    if( radius <= 0 )
      throw invalid_argument( "Radius must be positive.\n" );
    double density = loadDensity( circleDescription );
    TVector<2> position = loadVector2D( circleDescription[ "position" ].getArray() );
    CPhysicsObject *newObj = new CCircle( position, radius, density );
    newObj->addTag( tags );
    m_objects.push_back( newObj );
  }
  catch( const out_of_range & )
  {
    throw invalid_argument( "Circle must contain both size and position.\n" );
  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Circle position must be an array.\n" );
  }
}

void CLevelLoader::loadRectangle( const CJsonObject &rectDescription,
                                  ETag tags )
{
  try
  {
    TVector<2> position = loadVector2D( rectDescription[ "position" ].getArray() );
    TVector<2> size = loadVector2D( rectDescription[ "size" ].getArray() );
    if( size[ 0 ] <= 0 || size[ 1 ] <= 0 )
      throw invalid_argument( "Rectangle size must be positive.\n" );
    double rotation = loadRotation( rectDescription );
    double density = loadDensity( rectDescription );
    CPhysicsObject *newObj = new CRectangle( position, size,
                                             rotation, density );
    newObj->addTag( tags );
    m_objects.push_back( newObj );
  }
  catch( const out_of_range & )
  {
    throw invalid_argument( "Rectangle must contain both size and position.\n" );
  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Rectangle position and size must be an array.\n" );
  }
}

void CLevelLoader::loadText( const CJsonObject &textDescription, ETag tags )
{
  try
  {
    TVector<2> position = loadVector2D( textDescription[ "position" ].getArray() );
    string text = textDescription[ "text" ].toString();
    auto *newObj = new CText( position, text );
    newObj->addTag( tags );
    m_texts.push_back( newObj );
  }
  catch( const out_of_range & )
  {
    throw invalid_argument( "Text must contain both position and text.\n" );
  }
  catch( const bad_cast & )
  {
    throw invalid_argument( "Text position must be an array.\n" );
  }
}

double CLevelLoader::loadDensity( const CJsonObject &itemDescription )
{
  if( !itemDescription.count( "physics" ) || !itemDescription[ "physics" ].count( "density" ) )
    return HUGE_VAL;
  double density = itemDescription[ "physics" ][ "density" ].toDouble();
  if( density <= 0 )
    throw invalid_argument( "Density must be positive.\n" );
  return density;
}

double CLevelLoader::loadRotation( const CJsonObject &itemDescription )
{
  if( itemDescription.count( "rotation" ) )
    return itemDescription[ "rotation" ].toDouble();
  return 0;
}
