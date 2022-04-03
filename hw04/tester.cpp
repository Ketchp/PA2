#include "test.hpp"

#ifndef __PROGTEST__
bool writeTest( CFile &x, const initializer_list<uint8_t> &data )
{
  return x.write( data.begin(), data.size() ) == data.size();
}

bool readTest( CFile &x, const initializer_list<uint8_t> &data, uint32_t rdLen )
{
  uint8_t tmp[ 100 ];
  uint32_t idx = 0;

  if( x.read( tmp, rdLen ) != data.size () )
    return false;
  for( auto v : data )
    if( tmp[ idx++ ] != v )
      return false;
  return true;
}

int main ()
{
  CFile f0;

  assert( writeTest( f0, { 10, 20, 30 } ) ); // f0: 10 20 30 |
  assert( f0.fileSize() == 3 );
  assert( writeTest( f0, { 60, 70, 80 } ) ); // f0: 10 20 30 60 70 80 |
  assert( f0.fileSize() == 6 );
  assert( f0.seek( 2 ) ); // f0: 10 20 | 30 60 70 80
  assert( writeTest( f0, { 5, 4 } ) ); // f0: 10 20 | 5 4 70 80
  assert( f0.fileSize() == 6 );
  assert( f0.seek( 1 ) ); // f0: 10 | 20 5 4 70 80
  assert( readTest( f0, { 20, 5, 4, 70, 80 }, 7 ) );
  assert( f0.seek( 3 ) ); // f0: 10 20 5 | 4 70 80

  f0.addVersion(); // f0: 10 20 5 | 4 70 80

  assert( f0.seek( 6 ) ); // f0: 10 20 5 4 70 80 |
  assert( writeTest( f0, { 100, 101, 102, 103 } ) ); // f0: 10 20 5 4 70 80 100 101 102 103 |

  f0.addVersion(); // f0: 10 20 5 4 70 80 100 101 102 103 |

  assert( f0.seek( 5 ) ); // f0: 10 20 5 4 70 | 80 100 101 102 103
  CFile f1( f0 );
  f0.truncate(); // f0: 10 20 5 4 70 |
  assert( f0.seek( 0 ) ); // f0: | 10 20 5 4 70
  assert( readTest( f0, { 10, 20, 5, 4, 70 }, 20 ) );

  f0.printHistory();
  assert( f0.undoVersion() ); // f0: 10 20 5 4 70 80 100 101 102 103 |
  f0.printHistory();

  assert( f0.seek( 0 ) ); // f0: | 10 20 5 4 70 80 100 101 102 103
  assert( readTest( f0, { 10, 20, 5, 4, 70, 80, 100, 101, 102, 103 }, 20 ) );
  assert( f0.undoVersion() ); // f0: 10 20 5 | 4 70 80
  assert( f0.seek( 0 ) ); // f0: | 10 20 5 4 70 80
  assert( readTest( f0, { 10, 20, 5, 4, 70, 80 }, 20 ) );
  assert( !f0.seek( 100 ));
  assert( writeTest( f1, { 200, 210, 220 } ) ); // f1: 10 20 5 4 70 | 200 210 220 102 103
  assert( f1.seek( 0 ) ); // f1: | 10 20 5 4 70 200 210 220 102 103
  assert( readTest( f1, { 10, 20, 5, 4, 70, 200, 210, 220, 102, 103 }, 20 ) );
  assert( f1.undoVersion() ); // f1: 10 20 5 4 70 80 100 101 102 103 |
  assert( f1.undoVersion() ); // f1: 10 20 5 | 4 70 80
  assert( readTest( f1, { 4, 70, 80 }, 20 ) );
  assert( !f1.undoVersion() );

  CFile f2;
  CFile f3(f2);

  assert( writeTest( f2, { 1, 2, 3 } ) );
  assert( f2.seek(0) && readTest( f2, { 1, 2 }, 2 ) );


  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
