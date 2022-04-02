#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */


class CTimeStamp
{
public:
  CTimeStamp( int year, int month, int day, int hour, int minute, int second )
    : year( year ), month( month ), day( day ),
      hour( hour ), minute( minute ), second( second )
    {};

  friend bool operator<( const CTimeStamp &lhs, const CTimeStamp &rhs )
  {
    if( lhs.year < rhs.year ) return true;
    if( lhs.year > rhs.year ) return false;

    if( lhs.month < rhs.month ) return true;
    if( lhs.month > rhs.month ) return false;

    if( lhs.day < rhs.day ) return true;
    if( lhs.day > rhs.day ) return false;

    if( lhs.hour < rhs.hour ) return true;
    if( lhs.hour > rhs.hour ) return false;

    if( lhs.minute < rhs.minute ) return true;
    if( lhs.minute > rhs.minute ) return false;

    return lhs.second < rhs.second;
  };

  friend bool operator>( const CTimeStamp &lhs, const CTimeStamp &rhs )
  {
    return rhs < lhs;
  };

  friend bool operator<=( const CTimeStamp &lhs, const CTimeStamp &rhs )
  {
    return !( lhs > rhs );
  };

  friend bool operator>=( const CTimeStamp &lhs, const CTimeStamp &rhs )
  {
    return !( lhs < rhs );
  };


private:
  int year, month, day, hour, minute, second;
};


class CContact
{
public:
  CContact( const CTimeStamp &stamp, int newPerson1, int newPerson2 )
    : stamp( stamp ), person1( newPerson1 ), person2( newPerson2 )
  {
    if( person2 < person1 )
      std::swap( person1, person2 );
  };

  bool hasDistinctPeople() const
  {
    return person1 != person2;
  };

  std::pair<int, int> getPeople() const
  {
    return { person1, person2 };
  }

  bool contain( const int person ) const
  {
    return person == person1 || person == person2;
  };

  int other( const int person ) const
  {
    return person == person1 ? person2 : person1;
  };

private:
  CTimeStamp stamp;
  int person1, person2;
  friend class CEFaceMask;
};


class CEFaceMask
{
public:
  CEFaceMask &addContact( const CContact &newContact )
  {
    if( newContact.hasDistinctPeople() )
      contactList.insert( { newContact.stamp, newContact.getPeople() } );
    return *this;
  };

  std::vector<int> getSuperSpreaders( const CTimeStamp &from, const CTimeStamp &to ) const
  {
    std::set< std::pair<int, int> > timePeriodContacts;
    for( auto it = contactList.lower_bound( from );
         it->first <= to && it != contactList.end(); ++it )
      timePeriodContacts.insert( it->second );

    std::map<int, int> contacts;
    for( const auto &it: timePeriodContacts )
    {
      ++contacts[ it.first ];
      ++contacts[ it.second ];
    }
    int superContacts = std::max_element( contacts.cbegin(), contacts.cend(),
                  []( const std::pair<int, int> &lhs, const std::pair<int, int> &rhs )
                  {
                    return lhs.second < rhs.second;
                  } )->second;

    std::vector<int> superSpreaders;
    for( const auto &spreaderInfo: contacts )
      if( spreaderInfo.second == superContacts )
        superSpreaders.push_back( spreaderInfo.first );

    std::sort( superSpreaders.begin(), superSpreaders.end() );

    return superSpreaders;
  }

private:
  std::multimap< CTimeStamp, std::pair<int, int> > contactList;
  friend int main();
};


#ifndef __PROGTEST__
int main ()
{
  CEFaceMask test;

  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 10, 12, 40, 10 ), 111111111, 222222222 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 12, 12, 40, 10 ), 333333333, 222222222 ) )
       . addContact ( CContact ( CTimeStamp ( 2021, 2, 14, 15, 30, 28 ), 222222222, 444444444 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 2, 15, 18, 0, 0 ), 555555555, 444444444 ) );
  assert ( test . getSuperSpreaders ( CTimeStamp ( 2021, 1, 1, 0, 0, 0 ), CTimeStamp ( 2022, 1, 1, 0, 0, 0 ) ) == (vector<int> {222222222}) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 3, 20, 18, 0, 0 ), 444444444, 666666666 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 3, 25, 0, 0, 0 ), 111111111, 666666666 ) );
  assert ( test . getSuperSpreaders ( CTimeStamp ( 2021, 1, 1, 0, 0, 0 ), CTimeStamp ( 2022, 1, 1, 0, 0, 0 ) ) == (vector<int> {222222222, 444444444}) );
  return 0;
}
#endif /* __PROGTEST__ */
