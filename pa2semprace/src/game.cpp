#include "game.hpp"
#include "object.hpp"
#include "jsonParser.hpp"

game::game()
{
  m_window.init();

  m_window.registerDrawEvent( this, &game::update );
  m_window.registerTimerEvent( this, &game::nextFrame, 2000 );
  m_window.registerWindowCloseEvent( this, &game::gameEnd );
  // todo load levels

  JSON json( "assets/level_1.json" );
  if( json.m_type != jsonType::jsonObjectType )
    throw std::invalid_argument( "Level description must be json object." );

  jsonObject &levelDescription = dynamic_cast<jsonObject &>( json.get() );
  std::set<std::string> descriptionKeys = levelDescription.getKeys();

  if( descriptionKeys.count( "items" ) )
  {
    size_t objectCount = levelDescription[ "items" ].size();
    for( size_t idx = 0; idx < objectCount; ++idx )
    {
      std::set<std::string> itemAttributes = levelDescription[ "items" ][ idx ].getKeys();
      int id;
      std::string type;
      float x, y;
      if( itemAttributes.count( "id" ) )
        id = (int)levelDescription[ "items" ][ idx ][ "id" ];
      else throw std::invalid_argument( "Missing item id." );
      if( itemAttributes.count( "type" ) )
        type = (std::string)levelDescription[ "items" ][ idx ][ "type" ];
      else throw std::invalid_argument( "Missing item type." );
      if( itemAttributes.count( "position" ) )
      {
        if( levelDescription[ "items" ][ idx ][ "position" ].size() != 2 )
          throw std::invalid_argument( "Position expected array of two elements." );
        x = (float)levelDescription[ "items" ][ idx ][ "position" ][ 0 ];
        y = (float)levelDescription[ "items" ][ idx ][ "position" ][ 1 ];
      }
      else throw std::invalid_argument( "Missing item position." );
      alphaColor itemColor{ 255, 255, 255, 255 };
      if( itemAttributes.count( "color" ) )
      {
        size_t colSize = levelDescription[ "items" ][ idx ][ "color" ].size();
        if( colSize < 3 || 4 < colSize )
          throw std::invalid_argument( "Color expected array of three or four elements." );
        itemColor.red = (int)levelDescription[ "items" ][ idx ][ "color" ][ 0 ];
        itemColor.green = (int)levelDescription[ "items" ][ idx ][ "color" ][ 1 ];
        itemColor.blue = (int)levelDescription[ "items" ][ idx ][ "color" ][ 2 ];
        itemColor.alpha = colSize == 4 ? (int)levelDescription[ "items" ][ idx ][ "color" ][ 3 ] : 255;
      }

      if( type == "rectangle" )
      {
        float width, height;
        if( itemAttributes.count( "size" ) )
        {
          if( levelDescription[ "items" ][ idx ][ "size" ].size() != 2 )
            throw std::invalid_argument( "Rectangle size must be array of two elements." );
          width = (float)levelDescription[ "items" ][ idx ][ "size" ][ 0 ];
          height = (float)levelDescription[ "items" ][ idx ][ "size" ][ 1 ];
        }
        else throw std::invalid_argument( "Rectangle expected attribute size." );

        objects.emplace_back( std::unique_ptr<object>( new rectangle( id, x, y, width, height, itemColor ) ) );
        std::cout << "New item: "
        << id << ", "
        << x << ", "
        << y << ", "
        << width << ", "
        << height << std::endl;
      }
    }
  }
}

void game::nextFrame()
{
  std::cout << "Hurray" << std::endl;
  for( auto &item: objects )
  {
    dynamic_cast<rectangle *>(item.get())->m_position += vector2D{ 2, 2 };
  }
  m_window.registerTimerEvent( this, &game::nextFrame, 1000 );
  update();
}

void game::update()
{
  m_window.drawItems( objects );
}

void game::mainLoop()
{
  m_window.mainLoop();
}

void game::gameEnd()
{
  objects.resize( 0 );
}