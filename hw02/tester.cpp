#include "test.cpp"
#include <cassert>


#ifndef __PROGTEST__

bool maxFunc( const unsigned int a, const unsigned int b )
{
  return a > b;
}

struct triple
{
  string first, second, third;
  friend bool operator<( const triple &lhs, const triple &rhs )
  {
    if( caseInsCompare( lhs.first, rhs.first ) )
      return caseInsCompare( lhs.first, rhs.first ) < 0;
    return caseInsCompare( lhs.second, rhs.second ) < 0;
  };
  static int caseInsCompare( const string &lhs, const string &rhs )
  {
    auto itL = lhs.cbegin(), itR = rhs.cbegin();
    while( itL != lhs.cend() && itR != rhs.cend() )
    {
      if( tolower( *itL ) < tolower( *itR ) ) return -1;
      if( tolower( *itL ) > tolower( *itR ) ) return 1;
      ++itL; ++itR;
    }
    return (itL != lhs.cend()) - (itR != rhs.cend());
  };
};

void testOrder( CVATRegister db, vector<triple> &companies )
{
  sort( companies.begin(), companies.end() );
  string name, addr;

  if( companies.size() )
    assert( db.firstCompany( name, addr ) &&
            name == companies[0].first &&
            addr == companies[0].second );
  else
    assert( !db.firstCompany( name, addr ) );

  for( auto it = companies.cbegin() + 1; it != companies.cend(); ++it )
  {
    assert( db.nextCompany( name, addr ) &&
            name == it->first &&
            addr == it->second );
  }
  assert( !db.nextCompany( name, addr ) );
}

void testHeap()
{
  Heap hp( &maxFunc );

  assert ( hp.size() == 0 );
  assert ( hp.peak() == 0 );
  assert ( hp.pop() == 0 );

  hp.insert( 5 );
  assert ( hp.size() == 1 );
  assert ( hp.peak() == 5 );
  assert ( hp.pop() == 5 );

  assert ( hp.size() == 0 );
  assert ( hp.peak() == 0 );
  assert ( hp.pop() == 0 );

  const size_t MAXSIZE = 1000;
  for( size_t heapSize = 1; heapSize < MAXSIZE; heapSize += 10 )
  {
    for( unsigned int a = 0; a < heapSize; ++a )
    {
      hp.insert( a );
      assert ( hp.size() == a + 1 );
      assert ( hp.peak() == a );
    }

    for( unsigned int a = heapSize - 1; a < heapSize; --a )
    {
      assert ( hp.peak() == a );
      assert ( hp.pop() == a );
      assert ( hp.size() == a );
    }
  }
}

int main( void )
{
  testHeap();
  string name, addr;
  unsigned int sumIncome;

  {
  CVATRegister b1;
  assert ( b1 . newCompany ( "ACME", "Thakurova", "666/666" ) );
  assert ( b1 . newCompany ( "ACME", "Kolejni", "666/666/666" ) );
  assert ( b1 . newCompany ( "Dummy", "Thakurova", "123456" ) );
  assert ( b1 . invoice ( "666/666", 2000 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "666/666/666", 3000 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 4000 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "aCmE", "Kolejni", 5000 ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . audit ( "ACME", "Kolejni", sumIncome ) && sumIncome == 8000 );
  assert ( b1 . audit ( "123456", sumIncome ) && sumIncome == 4000 );
  assert ( b1 . firstCompany ( name, addr ) && name == "ACME" && addr == "Kolejni" );
  assert ( b1 . nextCompany ( name, addr ) && name == "ACME" && addr == "Thakurova" );
  assert ( b1 . nextCompany ( name, addr ) && name == "Dummy" && addr == "Thakurova" );
  assert ( ! b1 . nextCompany ( name, addr ) );
  assert ( b1 . cancelCompany ( "ACME", "KoLeJnI" ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . cancelCompany ( "666/666" ) );
  assert ( b1 . medianInvoice () == 4000 );
  assert ( b1 . invoice ( "123456", 100 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 300 ) );
  assert ( b1 . medianInvoice () == 3000 );
  assert ( b1 . invoice ( "123456", 200 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 230 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 830 ) );
  assert ( b1 . medianInvoice () == 830 );
  assert ( b1 . invoice ( "123456", 1830 ) );
  assert ( b1 . medianInvoice () == 1830 );
  assert ( b1 . invoice ( "123456", 2830 ) );
  assert ( b1 . medianInvoice () == 1830 );
  assert ( b1 . invoice ( "123456", 2830 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . invoice ( "123456", 3200 ) );
  assert ( b1 . medianInvoice () == 2000 );
  assert ( b1 . firstCompany ( name, addr ) && name == "Dummy" && addr == "Thakurova" );
  assert ( ! b1 . nextCompany ( name, addr ) );
  assert ( b1 . cancelCompany ( "123456" ) );
  assert ( ! b1 . firstCompany ( name, addr ) );
  }

  {
  CVATRegister b2;
  assert ( b2 . newCompany ( "ACME", "Kolejni", "abcdef" ) );
  assert ( b2 . newCompany ( "Dummy", "Kolejni", "123456" ) );
  assert ( ! b2 . newCompany ( "AcMe", "kOlEjNi", "1234" ) );
  assert ( b2 . newCompany ( "Dummy", "Thakurova", "ABCDEF" ) );
  assert ( b2 . medianInvoice () == 0 );
  assert ( b2 . invoice ( "ABCDEF", 1000 ) );
  assert ( b2 . medianInvoice () == 1000 );
  assert ( b2 . invoice ( "abcdef", 2000 ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( b2 . invoice ( "aCMe", "kOlEjNi", 3000 ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( ! b2 . invoice ( "1234567", 100 ) );
  assert ( ! b2 . invoice ( "ACE", "Kolejni", 100 ) );
  assert ( ! b2 . invoice ( "ACME", "Thakurova", 100 ) );
  assert ( ! b2 . audit ( "1234567", sumIncome ) );
  assert ( ! b2 . audit ( "ACE", "Kolejni", sumIncome ) );
  assert ( ! b2 . audit ( "ACME", "Thakurova", sumIncome ) );
  assert ( ! b2 . cancelCompany ( "1234567" ) );
  assert ( ! b2 . cancelCompany ( "ACE", "Kolejni" ) );
  assert ( ! b2 . cancelCompany ( "ACME", "Thakurova" ) );
  assert ( b2 . cancelCompany ( "abcdef" ) );
  assert ( b2 . medianInvoice () == 2000 );
  assert ( ! b2 . cancelCompany ( "abcdef" ) );
  assert ( b2 . newCompany ( "ACME", "Kolejni", "abcdef" ) );
  assert ( b2 . cancelCompany ( "ACME", "Kolejni" ) );
  assert ( ! b2 . cancelCompany ( "ACME", "Kolejni" ) );
  }


  CVATRegister db;
  vector<triple> companies
  {
    {"a","a","99"},
    {"b","a","98"},
    {"c","a","97"},
    {"d","a","96"},
    {"e","a","95"},
    {"f","a","94"},
    {"g","a","93"},
    {"h","a","92"},
    {"i","a","91"},
    {"j","a","90"},
    {"k","a","89"},
    {"l","a","88"},
    {"m","a","87"},
    {"n","a","86"},
    {"o","a","85"},
    {"p","a","84"},
    {"q","a","83"},
    {"r","a","82"}
  };
  
  for( const auto &triple : companies )
  {
    db.newCompany( triple.first, triple.second, triple.third );
  }

  testOrder( db, companies );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
