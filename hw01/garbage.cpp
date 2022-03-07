/*
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
  ifstream istream( "tests/test0.ori" );
  cout << istream.good() << istream.eof() << istream.bad() << istream.fail() << endl;
  if( !istream.good() )
    return 0;



  while( true )
  {
    char temp;
    istream.read( &temp, 1 );
    if( !istream.good() ) break;
    cout << temp << endl;
  }

  cout << "----" << endl;

  cout << istream.good() << istream.eof() << istream.bad() << istream.fail() << endl;

  char temp;
  istream.read( &temp, 1 );

  cout << istream.good() << istream.eof() << istream.bad() << istream.fail() << endl;


}
*/