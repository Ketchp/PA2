#include "../../src/jsonParser.hpp"
#include "../../src/linearAlgebra.hpp"
#include <cassert>

void jsonParserTest()
{
  {
    CJsonDocument obj( "jsonTests/example_1.json" );

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
    CJsonDocument obj( "jsonTests/example_level_1.json" );
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
  {
    CJsonDocument doc( "jsonTests/i_number_double_huge_neg_exp.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_huge_exp.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      std::cout << num << std::endl;
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_neg_int_huge_exp.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_pos_double_huge_exp.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_real_neg_overflow.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_real_pos_overflow.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_real_underflow.json" );
    try
    {
      double num = doc.get()[0].toDouble();
      assert( num && "Shouldn't got here" == nullptr );
    }
    catch( const std::out_of_range & )
    {}
  }
  {
    CJsonDocument doc( "jsonTests/i_number_too_big_neg_int.json" );
    double num = doc.get()[0].toDouble();
    assert( num == -123123123123123123123123123123. );
  }
  {
    CJsonDocument doc( "jsonTests/i_number_too_big_pos_int.json" );
    double num = doc.get()[0].toDouble();
    assert( num == 100000000000000000000. );
  }
  {
    CJsonDocument doc( "jsonTests/i_number_very_big_negative_int.json" );
    double num = doc.get()[0].toDouble();
    assert( num == -237462374673276894279832749832423479823246327846. );
  }
  {
    CJsonDocument doc( "jsonTests/i_structure_500_nested_arrays.json" );
    const CJsonArray &arr = doc.get().getArray()[0].getArray()[0].getArray();
    assert( arr.m_type == EJsonType::jsonArrayType );
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/i_structure_UTF-8_BOM_empty_object.json" );
      const CJsonObject &obj = doc.get().getObject();
      assert( obj.size() && "Shouldn't got here" == nullptr );
    }
    catch( const std::invalid_argument & )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_1_true_without_comma.json" );
      assert( "Missing comma" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_colon_instead_of_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_comma_after_close.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_comma_and_number.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_double_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_double_extra_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_extra_close.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_extra_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_incomplete.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_incomplete_invalid_value.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_inner_array_no_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_items_separated_by_semicolon.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_just_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_just_minus.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_missing_value.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_newlines_unclosed.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_number_and_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_number_and_several_commas.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_newlines_unclosed.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_star_inside.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_unclosed.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_unclosed_trailing_comma.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_unclosed_with_new_lines.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_array_unclosed_with_object_inside.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_incomplete_false.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_incomplete_null.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_incomplete_true.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_++.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_+1.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_+Inf.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_.2e-3.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_.-1.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0.1.2.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0.3e+.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0.3e.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0.e1.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0_capital_E+.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0_capital_E.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0e+.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_0e.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_1.0e+.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_1.0e.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_1.0e-.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_1_000.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_1eE2.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_2.e+3.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_2.e3.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_2.e-3.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_9.e+.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_-1.0..json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_-01.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_-2..json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_-NaN.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_expression.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_hex_1_digit.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_hex_2_digits.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_Inf.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_infinity.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_invalid+-.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_invalid-negative-real.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_minus_infinity.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_minus_sign_with_trailing_garbage.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_minus_space_1.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
  }
  {
    try
    {
      CJsonDocument doc( "jsonTests/n_number_NaN.json" );
      assert( "Wrong JSON" == nullptr );
    }
    catch( const std::invalid_argument &e )
    {}
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