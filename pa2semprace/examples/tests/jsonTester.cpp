#include "../../src/jsonParser.hpp"
#include <cassert>

int main()
{
  {
    JSON obj( "example_1.json" );

    jsonObject &object1 = dynamic_cast<jsonObject &>( obj.get() );

    assert( obj.m_type == jsonType::jsonObjectType );
    assert( object1.m_type == jsonType::jsonObjectType );

    assert( ( object1.getKeys() == std::set<std::string>{ "glossary" } ) );
    assert( ( object1.getKeys() == std::set<std::string>{ "glossary" } ) );

    assert( object1[ "glossary" ].size() == 2 );
    assert( object1[ "glossary" ].size() == 2 );

    jsonObject &object2 = dynamic_cast<jsonObject &>( object1[ "glossary" ] );

    assert( ( object2.getKeys() == std::set<std::string>{ "title", "GlossDiv" } ) );
    assert( (std::string)object2[ "title" ] == "example glossary" );

    jsonObject &object3 = dynamic_cast<jsonObject &>( object2[ "GlossDiv" ] );
    assert( ( object3.getKeys() == std::set<std::string>{ "title", "GlossList" } ) );
    assert( (std::string)object3[ "title" ] == "S" );

    jsonObject &object4 = dynamic_cast<jsonObject &>( object3[ "GlossList" ] );
    assert( ( object4.getKeys() == std::set<std::string>{ "GlossEntry" } ) );

    jsonObject &object5 = dynamic_cast<jsonObject &>( object4[ "GlossEntry" ] );
    assert( ( object5.getKeys() == std::set<std::string>{ "ID", "SortAs", "GlossTerm",
                                                          "Acronym", "Abbrev",
                                                          "GlossDef", "GlossSee" } ) );

    assert( (std::string)( object5 )[ "ID" ] == "SGML" );
    assert( (std::string)( object5 )[ "SortAs" ] == "SGML" );
    assert( (std::string)( object5 )[ "GlossTerm" ] == "Standard Generalized Markup Language" );
    assert( (std::string)( object5 )[ "Acronym" ] == "SGML" );
    assert( (std::string)( object5 )[ "Abbrev" ] == "ISO 8879:1986" );
    assert( (std::string)( object5 )[ "GlossSee" ] == "markup" );

    jsonObject &object6 = dynamic_cast<jsonObject &>( object5[ "GlossDef" ] );
    assert( ( object6.getKeys() == std::set<std::string>{ "para", "GlossSeeAlso" } ) );

    assert( (std::string)object6[ "para" ] == "A meta-markup language, used to create markup languages such as DocBook." );

    jsonArray &array1 = dynamic_cast<jsonArray &>( object6[ "GlossSeeAlso" ] );

    assert( array1.size() == 2 );
    assert( (std::string)( array1[ 0 ] ) == "GML" );
    assert( (std::string)( array1[ 1 ] ) == "XML" );
  }
  {
    JSON obj( "example_level_1.json" );
    jsonObject &value = dynamic_cast<jsonObject &>( obj.get() );
    assert( ( value.getKeys() == std::set<std::string>{ "screen", "items" } ) );

    jsonObject &screen = dynamic_cast<jsonObject &>( value[ "screen" ] );
    assert( ( screen.getKeys() == std::set<std::string>{ "size", "bg_color" } ) );

    assert( (int)(screen[ "size" ][ 0 ]) == 1000 );
    assert( (float)(screen[ "size" ][ 0 ]) == 1000.f );
    assert( (double)(screen[ "size" ][ 0 ]) == 1000.0 );
    assert( (int)(screen[ "size" ][ 1 ]) == 800 );

    assert( (int)(screen[ "bg_color" ][ 0 ]) == 200 );
    assert( (int)(screen[ "bg_color" ][ 1 ]) == 200 );
    assert( (int)(screen[ "bg_color" ][ 2 ]) == 200 );
    assert( (int)(screen[ "bg_color" ][ 3 ]) == 255 );

    jsonArray &items = dynamic_cast<jsonArray &>( value[ "items" ] );
    assert( items.size() == 3 );
    assert( ( items[ 0 ].getKeys() == std::set<std::string>{ "id", "type", "position", "size", "color" } ) );
    assert( ( items[ 1 ].getKeys() == std::set<std::string>{ "id", "type", "position", "size", "color" } ) );
    assert( ( items[ 2 ].getKeys() == std::set<std::string>{ "id", "type", "position", "size" } ) );

    assert( (int)items[ 0 ][ "id" ] == 1 );
    assert( (int)items[ 1 ][ "id" ] == 2 );
    assert( (int)items[ 2 ][ "id" ] == 3 );

    assert( (std::string)items[ 0 ][ "type" ] == "rectangle" );
    assert( (std::string)items[ 1 ][ "type" ] == "rectangle" );
    assert( (std::string)items[ 2 ][ "type" ] == "circle" );

    assert( (int)items[ 0 ][ "position" ][0] == 500 );
    assert( (int)items[ 0 ][ "position" ][1] == 780 );
    assert( (int)items[ 1 ][ "position" ][0] == 500 );
    assert( (int)items[ 1 ][ "position" ][1] ==  20 );
    assert( (int)items[ 2 ][ "position" ][0] == 500 );
    assert( (int)items[ 2 ][ "position" ][1] == 400 );

    assert( (int)items[ 0 ][ "size" ][ 0 ] == 1000 );
    assert( (int)items[ 0 ][ "size" ][ 1 ] == 20  );
    assert( (int)items[ 1 ][ "size" ][ 0 ] == 1000 );
    assert( (int)items[ 1 ][ "size" ][ 1 ] == 20  );
    assert( (int)items[ 2 ][ "size" ] == 50  );

    assert( (int)items[ 0 ][ "color" ][ 0 ] == 255 );
    assert( (int)items[ 0 ][ "color" ][ 1 ] ==   0 );
    assert( (int)items[ 0 ][ "color" ][ 2 ] ==   0 );
    assert( (int)items[ 0 ][ "color" ][ 3 ] == 127 );
    assert( (int)items[ 1 ][ "color" ][ 0 ] ==   0 );
    assert( (int)items[ 1 ][ "color" ][ 1 ] == 255 );
    assert( (int)items[ 1 ][ "color" ][ 2 ] ==   0 );
    assert( (int)items[ 1 ][ "color" ][ 3 ] == 127 );

  }

}