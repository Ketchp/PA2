#include "../../src/jsonParser.hpp"
#include "../../src/linearAlgebra.hpp"
#include <cassert>

void jsonParserTest()
{
  {
    CJsonDocument obj( "example_1.json" );

    auto &object1 = obj.get().getObject();

    assert( obj.m_type == EJsonType::jsonObjectType );
    assert( object1.m_type == EJsonType::jsonObjectType );

    assert( ( object1.count( "glossary" ) == 1 ) );

    assert( object1[ "glossary" ].size() == 2 );

    auto &object2 = object1[ "glossary" ].getObject();

    assert( ( object2.count( "title" ) == 1 ) );
    assert( ( object2.count( "GlossDiv" ) == 1 ) );

    assert( object2[ "title" ].toString() == "example glossary" );

    auto &object3 = object2[ "GlossDiv" ].getObject();

    assert( ( object3.count( "title" ) == 1 ) );
    assert( ( object3.count( "GlossList" ) == 1 ) );
    assert( ( object3[ "title" ].toString() == "S" ) );

    auto &object4 = object3[ "GlossList" ].getObject();

    assert( ( object4.count( "GlossEntry" ) == 1 ) );

    auto &object5 = object4[ "GlossEntry" ].getObject();

    assert( object5[ "ID" ].toString() == "SGML" );
    assert( object5[ "SortAs" ].toString() == "SGML" );
    assert( object5[ "GlossTerm" ].toString() == "Standard Generalized Markup Language" );
    assert( object5[ "Acronym" ].toString() == "SGML" );
    assert( object5[ "Abbrev" ].toString() == "ISO 8879:1986" );
    assert( object5[ "GlossSee" ].toString() == "markup" );

    auto &object6 = object5[ "GlossDef" ].getObject();

    assert( object6[ "para" ].toString() == "A meta-markup language, used to create markup languages such as DocBook." );

    auto &array1 = object6[ "GlossSeeAlso" ].getArray();

    assert( array1.size() == 2 );
    assert( array1[ 0 ].toString() == "GML" );
    assert( array1[ 1 ].toString() == "XML" );
  }
  {
    CJsonDocument obj( "example_level_1.json" );
    auto &value = obj.get().getObject();

    auto &screen = value[ "screen" ].getObject();

    assert( screen[ "size" ][ 0 ].toInt() == 1000 );
    assert( screen[ "size" ][ 0 ].toDouble() == 1000. );
    assert( screen[ "size" ][ 1 ].toInt() == 800 );

    assert( screen[ "bg_color" ][ 0 ].toInt() == 200 );
    assert( screen[ "bg_color" ][ 1 ].toInt() == 200 );
    assert( screen[ "bg_color" ][ 2 ].toInt() == 200 );
    assert( screen[ "bg_color" ][ 3 ].toInt() == 255 );

    auto &items = value[ "items" ].getArray();
    assert( items.size() == 3 );

    assert( items[ 0 ][ "id" ].toInt() == 1 );
    assert( items[ 1 ][ "id" ].toInt() == 2 );
    assert( items[ 2 ][ "id" ].toInt() == 3 );

    assert( items[ 0 ][ "type" ].toString() == "rectangle" );
    assert( items[ 1 ][ "type" ].toString() == "rectangle" );
    assert( items[ 2 ][ "type" ].toString() == "circle" );

    assert( items[ 0 ][ "position" ][0].toInt() == 500 );
    assert( items[ 0 ][ "position" ][1].toInt() == 780 );
    assert( items[ 1 ][ "position" ][0].toInt() == 500 );
    assert( items[ 1 ][ "position" ][1].toInt() ==  20 );
    assert( items[ 2 ][ "position" ][0].toInt() == 500 );
    assert( items[ 2 ][ "position" ][1].toInt() == 400 );

    assert( items[ 0 ][ "size" ][ 0 ].toInt() == 1000 );
    assert( items[ 0 ][ "size" ][ 1 ].toInt() == 20  );
    assert( items[ 1 ][ "size" ][ 0 ].toInt() == 1000 );
    assert( items[ 1 ][ "size" ][ 1 ].toInt() == 20  );
    assert( items[ 2 ][ "size" ].toInt() == 50  );

    assert( items[ 0 ][ "color" ][ 0 ].toInt() == 255 );
    assert( items[ 0 ][ "color" ][ 1 ].toInt() ==   0 );
    assert( items[ 0 ][ "color" ][ 2 ].toInt() ==   0 );
    assert( items[ 0 ][ "color" ][ 3 ].toInt() == 127 );
    assert( items[ 1 ][ "color" ][ 0 ].toInt() ==   0 );
    assert( items[ 1 ][ "color" ][ 1 ].toInt() == 255 );
    assert( items[ 1 ][ "color" ][ 2 ].toInt() ==   0 );
    assert( items[ 1 ][ "color" ][ 3 ].toInt() == 127 );
  }
}

template <size_t dim>
bool compareVectors( const TVector<dim> &first, const TVector<dim> &second )
{
  for( size_t idx = 0; idx < dim; ++idx )
    if( !equalDoubles( first[ idx ], second[ idx ] ) )
      return false;
  return true;
}

template <size_t h, size_t w>
bool compareMatrices( const TMatrix<h, w> &first, const TMatrix<h, w> &second )
{
  for( size_t idx = 0; idx < w; ++idx )
    if( !compareVectors( first[ idx ], second[ idx ] ) )
      return false;
  return true;
}

void linearAlgebraTest()
{
  using vec2d = TVector<2>;
  assert( compareVectors( vec2d{ 2, 2 }.projectTo( { 0, 1 } ), vec2d{ 0, 2 } ) );
  assert( compareVectors( vec2d{ 2, 2 }.rejectFrom( { 0, 1 } ), vec2d{ 2, 0 } ) );
  assert( ( vec2d{ 5, 6 }.dot( { 3, 2 } ) == 27. ) );

  TMatrix<2,2> m1{ { 2, 3 }, { 3, 4 } };
  assert( m1.invert() );
  assert( compareMatrices( m1, TMatrix<2,2>{ { -4, 3 }, { 3, -2 } } ) );

  TMatrix<2,2> m2{ { 4, 1 }, { 11, 3 } };
  assert( m2.invert() );
  assert( compareMatrices( m2, TMatrix<2,2>{ { 3, -1 }, { -11, 4 } } ) );


}

int main()
{
  jsonParserTest();
  linearAlgebraTest();
}