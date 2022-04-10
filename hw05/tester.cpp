#include "test.hpp"

int main ()
{
  CSupermarket s;
  s . store ( "bread", CDate ( 2016, 4, 30 ), 100 )
          . store ( "butter", CDate ( 2016, 5, 10 ), 10 )
          . store ( "beer", CDate ( 2016, 8, 10 ), 50 )
          . store ( "bread", CDate ( 2016, 4, 25 ), 100 )
          . store ( "okey", CDate ( 2016, 7, 18 ), 5 );
  // { bread:100+100, beer:50, butter:10, okey:5 }
  list<pair<string,int> > l0 = s . expired ( CDate ( 2018, 4, 30 ) );
  assert ( l0 . size () == 4 );
  assert ( ( l0 == list<pair<string,int> > { { "bread", 200 }, { "beer", 50 }, { "butter", 10 }, { "okey", 5 } } ) );

  // { bread:98+100, beer:50, okey:5 }
  list<pair<string,int> > l1 { { "bread", 2 }, { "Coke", 5 }, { "butter", 20 } };
  s . sell ( l1 );
  assert ( l1 . size () == 2 );
  assert ( ( l1 == list<pair<string,int> > { { "Coke", 5 }, { "butter", 10 } } ) );

  list<pair<string,int> > l2 = s . expired ( CDate ( 2016, 4, 30 ) );
  assert ( l2 . size () == 1 );
  assert ( ( l2 == list<pair<string,int> > { { "bread", 98 } } ) );

  list<pair<string,int> > l3 = s . expired ( CDate ( 2016, 5, 20 ) );
  assert ( l3 . size () == 1 );
  assert ( ( l3 == list<pair<string,int> > { { "bread", 198 } } ) );

  // { bread:93, beer:50, okey:5 }
  list<pair<string,int> > l4 { { "bread", 105 } };
  s . sell ( l4 );
  assert ( l4 . empty () );

  list<pair<string,int> > l5 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l5 . size () == 3 );
  assert ( ( l5 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 } } ) );

  // { bread:93, beer:50, okey:5, Coke:10 }
  s . store ( "Coke", CDate ( 2016, 12, 31 ), 10 );

  list<pair<string,int> > l6 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 }, { "Cokes", 1 } };
  s . sell ( l6 );
  assert ( l6 . size () == 3 );
  assert ( ( l6 == list<pair<string,int> > { { "cake", 1 }, { "cuke", 1 }, { "Cokes", 1 } } ) );

  list<pair<string,int> > l7 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l7 . size () == 4 );
  assert ( ( l7 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 7 }, { "okey", 5 } } ) );

  s . store ( "cake", CDate ( 2016, 11, 1 ), 5 );

  list<pair<string,int> > l8 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 } };
  s . sell ( l8 );
  assert ( l8 . size () == 2 );
  assert ( ( l8 == list<pair<string,int> > { { "Cake", 1 }, { "coke", 1 } } ) );

  list<pair<string,int> > l9 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l9 . size () == 5 );
  assert ( ( l9 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 }, { "cake", 3 } } ) );

  list<pair<string,int> > l10 { { "cake", 15 }, { "Cake", 2 } };
  s . sell ( l10 );
  assert ( l10 . size () == 2 );
  assert ( ( l10 == list<pair<string,int> > { { "cake", 12 }, { "Cake", 2 } } ) );

  list<pair<string,int> > l11 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l11 . size () == 4 );
  assert ( ( l11 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 } } ) );

  list<pair<string,int> > l12 { { "Cake", 4 } };
  s . sell ( l12 );
  assert ( l12 . empty () );

  list<pair<string,int> > l13 = s . expired ( CDate ( 2017, 1, 1 ) );
  assert ( l13 . size () == 4 );
  assert ( ( l13 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 }, { "Coke", 2 } } ) );

  list<pair<string,int> > l14 { { "Beer", 20 }, { "Coke", 1 }, { "bear", 25 }, { "beer", 10 } };
  s . sell ( l14 );
  assert ( l14 . size () == 1 );
  assert ( ( l14 == list<pair<string,int> > { { "beer", 5 } } ) );

  s . store ( "ccccb", CDate ( 2019, 3, 11 ), 100 )
          . store ( "ccccd", CDate ( 2019, 6, 9 ), 100 )
          . store ( "dcccc", CDate ( 2019, 2, 14 ), 100 );

  list<pair<string,int> > l15 { { "ccccc", 10 } };
  s . sell ( l15 );
  assert ( l15 . size () == 1 );
  assert ( ( l15 == list<pair<string,int> > { { "ccccc", 10 } } ) );

  CSupermarket s1;

  s1.store( "a", CDate( 2020, 5, 5 ), 20 );
  list<pair<string,int>> ll = { { "a", 18 }, { "b", 1 }, { "c", 2 } };
  s1.sell( ll );
  assert( ll.size() == 1 );
  assert( ( ll == list<pair<string,int>> { { "c", 1 } } ) );

  s1.store( "b", CDate( 2020, 5, 5 ), 20 );
  ll = { { "a", 18 }, { "c", 5 }, { "b", 2 } };
  s1.sell( ll );
  assert( ll.size() == 2 );
  assert( ( ll == list<pair<string,int>> { { "c", 3 }, { "b", 2 } } ) );

  ll = { { "", 1 } };
  s1.sell( ll );
  assert( ll.size() == 1 );
  assert( ( ll == list<pair<string,int>> { { "", 1 } } ) );

  s1.store( "", CDate( 2020, 5, 6 ), 1 );
  ll = { { "", 3 } };
  s1.sell( ll );
  assert( ll.size() == 1 );
  assert( ( ll == list<pair<string,int>> { { "", 2 } } ) );

  ll = { { "0123456789", 1 } };
  s1.sell( ll );

  auto &&expr = fuzzyString( "" );
  assert( ( expr.begin() == expr.end() ) );

  array<string, 1> arr1{ "" };
  for( const auto &[ fuzzy, idx ]: fuzzyString( "a" ) )
    assert( fuzzy == arr1[ idx ] );

  array<string, 5> arr2{ "bcde", "acde", "abde", "abce", "abcd" };
  for( const auto &[ fuzzy, idx ]: fuzzyString( "abcde" ) )
    assert( fuzzy == arr2[ idx ] );


  return EXIT_SUCCESS;
}
