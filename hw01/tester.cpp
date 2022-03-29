#include "test.hpp"

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
  ifstream f1 ( fileName1, ifstream::binary | ifstream::ate );
  ifstream f2 ( fileName2, ifstream::binary | ifstream::ate );

  if( f1.fail() || f2.fail() )
    return false; // file problem

  if( f1.tellg() != f2.tellg() )
    return false; // size mismatch

  // seek back to beginning and use std::equal to compare contents
  f1.seekg( 0, ifstream::beg );
  f2.seekg( 0, ifstream::beg );
  return equal( istreambuf_iterator<char>( f1.rdbuf() ),
                istreambuf_iterator<char>(),
                istreambuf_iterator<char>( f2.rdbuf() ));
}

int main ( void )
{
  assert( ones( 0 ) == 0ull );
  assert( ones( 1 ) == 1ull );
  assert( ones( 63 ) == UINT64_MAX >> 1 );
  assert( ones( 64 ) == UINT64_MAX );
/*
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test0.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test1.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );
*/
  assert ( compressFile ( "tests/test2.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test3.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( compressFile ( "tests/test4.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra0.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra1.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra2.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra3.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra4.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra5.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra6.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra7.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra8.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  assert ( compressFile ( "tests/extra9.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  assert ( !decompressFile ( "tests/in_4531057.bin", "tempfile" ) );

  assert ( compressFile ( "tests/random1-utf8.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/random1-utf8.orig", "tempfile" ) );

  assert ( compressFile ( "tests/random2-utf8.orig", "tempfile1" ) );
  assert ( decompressFile ( "tempfile1", "tempfile" ) );
  assert ( identicalFiles ( "tests/random2-utf8.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
