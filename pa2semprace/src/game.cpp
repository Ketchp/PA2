#include "game.hpp"
#include "object.hpp"

CGame::CGame( int *argcPtr, char *argv[] )
  : m_window( argcPtr, argv )
{

  m_engine.addField( CForceField::gravitationalField( 10 ) );

  loadLevel( "assets/level_1.json" );

  m_window.registerDrawEvent( this, &CGame::redraw );
  m_window.registerMouseButtonEvent( this, &CGame::mouseClick );
  m_window.registerMotionButtonEvent( this, &CGame::mouseMove );
  m_window.registerTimerEvent( this, &CGame::nextFrame, 2000 );
}

void CGame::loadLevel( const std::string &levelFileName )
{
  JSON json( levelFileName );
  if( json.m_type != jsonType::jsonObjectType )
    throw std::invalid_argument( "Level description must be json object.\n" );

  auto &levelDescription = dynamic_cast<jsonObject &>( json.get() );

  if( levelDescription.count( "screen" ) )
    loadLevelScreen( dynamic_cast<const jsonObject &>(levelDescription[ "screen" ]) );

  if( levelDescription.count( "items" ) )
    loadItems( dynamic_cast<const jsonArray &>(levelDescription[ "items" ]) );

}

void CGame::loadLevelScreen( const jsonObject &screenDescription )
{
  std::set<std::string> attributes = screenDescription.getKeys();

  math::vector levelScreenSize{ 800, 600 };
  if( attributes.count( "size" ) )
    levelScreenSize = loadLevelSize( dynamic_cast<const jsonArray &>( screenDescription[ "size" ] ) );
  m_window.resizeView( 0, levelScreenSize[ 0 ], 0, levelScreenSize[ 1 ] );

  // todo load color

}

math::vector CGame::loadLevelSize( const jsonArray &levelSizeDescription )
{
  if( levelSizeDescription.size() != 2 )
    throw std::invalid_argument( "Screen size array must have size 2.\n" );
  return { (double)levelSizeDescription[ 0 ],
           (double)levelSizeDescription[ 1 ] };
}


void CGame::loadItems( const jsonArray &itemsDescription )
{
  size_t objectCount = itemsDescription.size();
  for( size_t idx = 0; idx < objectCount; ++idx )
    loadItem( dynamic_cast<const jsonObject &>( itemsDescription[ idx ] ) );
}

void CGame::loadItem( const jsonObject &itemDescription )
{
  std::set<std::string> itemAttributes = itemDescription.getKeys();
//  int id = loadItemId( itemDescription );
  std::string type = loadItemType( itemDescription );
  if( type == "bounding box" )
  {
    math::vector screenSize = m_window.getViewSize();
    physicsAttributes immovable( HUGE_VAL );

    m_physicsObjects.emplace_back( (CObject *) new CComplexObject(
            CComplexObject::makeRectangle( screenSize / 2,
                                           0.999 * screenSize,
                                           immovable ) ) );

    return;
  }

  math::vector position = loadItemPosition( itemDescription );

  /*
  alphaColor itemColor{ 255, 255, 255, 255 };
  if( itemAttributes.count( "color" ) )
    itemColor = loadItemColor( itemDescription );
  */

  physicsAttributes physicsProperties( HUGE_VAL );
  if( itemAttributes.count( "physics" ) )
    physicsProperties = loadItemPhysics( dynamic_cast<const jsonObject &>( itemDescription[ "physics" ] ) );

  if( type == "rectangle" )
  {
    math::vector size = loadItemSize2D( itemDescription );

    m_physicsObjects.emplace_back( (CObject *) new CComplexObject(
            CComplexObject::makeRectangle( position,
                                           size,
                                           physicsProperties ) ) );
  }
  else if( type == "circle" )
  {
    double size = loadItemSize1D( itemDescription );
    m_physicsObjects.emplace_back( (CObject *)( new CCircle( position,
                                                             size,
                                                             physicsProperties ) ) );
  }
}

physicsAttributes CGame::loadItemPhysics( const jsonObject &itemDescription )
{
  double mass = (double)itemDescription["mass"];
  return physicsAttributes{ mass };
}

std::string CGame::loadItemType( const jsonObject &itemDescription )
{
  try
  {
    return (std::string)itemDescription[ "type" ];
  }
  catch( std::invalid_argument & )
  {
    throw std::invalid_argument( "Missing item type.\n" );
  }
}

math::vector CGame::loadItemPosition( const jsonObject &itemDescription )
{
  try
  {
    auto itemPosition = dynamic_cast<const jsonArray &>( itemDescription[ "position" ] );
    if( itemPosition.size() != 2 )
      throw std::invalid_argument( "Item position array must be size 2.\n" );
    return { (double)itemPosition[ 0 ],
             (double)itemPosition[ 1 ] };
  }
  catch( std::invalid_argument &e )
  {
    std::cerr << e.what();
    throw std::invalid_argument( "Expected item position array.\n" );
  }
}

double CGame::loadItemSize1D( const jsonObject &itemDescription )
{
  try
  {
    return (double)itemDescription[ "size" ];
  }
  catch( std::invalid_argument & )
  {
    throw std::invalid_argument( "Item must contain size field.\n" );
  }
}

math::vector CGame::loadItemSize2D( const jsonObject &itemDescription )
{
  try
  {
    auto itemSizeDescription = dynamic_cast<const jsonArray &>( itemDescription[ "size" ] );
    if( itemSizeDescription.size() != 2 )
      throw std::invalid_argument( "Item size must by array of size 2.\n" );
    return { (double)itemSizeDescription[ 0 ],
             (double)itemSizeDescription[ 1 ] };
  }
  catch( std::invalid_argument &e )
  {
    std::cerr << e.what();
    throw std::invalid_argument( "Wrong/no size field.\n" );
  }
}



void CGame::nextFrame()
{
  m_engine.step( m_physicsObjects );
  m_window.registerTimerEvent( this, &CGame::nextFrame, 100 );
  redraw();
}

void CGame::redraw()
{
  m_window.drawItems( m_physicsObjects );
}

void CGame::mainLoop()
{
  m_window.mainLoop();
}

void CGame::mouseClick( int button, int state, int x, int y )
{
  if( button == GLUT_LEFT_BUTTON )
  {
    if( state == GLUT_DOWN )
    {
      lastMousePosition = { x, y };
    }
  }
  std::cout << "Click: [ " << x << ", " << y << " ]" << std::endl;
}

void CGame::mouseMove( int x, int y )
{
  if( lastMousePosition.distance( { x, y } ) > 5 )
  {
    m_physicsObjects.emplace_back( (CObject *) new CLine( lastMousePosition, { x, y }, physicsAttributes( HUGE_VAL ) ) );
    lastMousePosition = { x, y };
  }
}