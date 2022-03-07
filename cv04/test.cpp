#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
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

  bool operator<( const CTimeStamp &other ) const
  {
    if( year < other.year ) return true;
    if( year > other.year ) return false;

    if( month < other.month ) return true;
    if( month > other.month ) return false;

    if( day < other.day ) return true;
    if( day > other.day ) return false;

    if( hour < other.hour ) return true;
    if( hour > other.hour ) return false;

    if( minute < other.minute ) return true;
    if( minute > other.minute ) return false;

    return second < other.second;
  };

  bool operator>( const CTimeStamp &other ) const
  {
    if( year > other.year ) return true;
    if( year < other.year ) return false;

    if( month > other.month ) return true;
    if( month < other.month ) return false;

    if( day > other.day ) return true;
    if( day < other.day ) return false;

    if( hour > other.hour ) return true;
    if( hour < other.hour ) return false;

    if( minute > other.minute ) return true;
    if( minute < other.minute ) return false;

    return second > other.second;
  };

private:
  int year, month, day, hour, minute, second;
};

class CContact
{
public:
  CContact( const CTimeStamp &stamp, int person1, int person2 )
    : stamp( stamp ), person1( person1 ), person2( person2 )
  {};

  CContact( CTimeStamp &&stamp, int person1, int person2 )
    : stamp( stamp ), person1( person1 ), person2( person2 )
  {};

  bool hasDistinctPeople() const
  {
    return person1 != person2;
  };

  bool contain( const int person ) const
  {
    return person == person1 || person == person2;
  };

  int other( const int person ) const
  {
    return person == person1 ? person2 : person1;
  };

  bool operator<( const CTimeStamp &time ) const
  {
    return stamp < time;           
  };

  bool operator>( const CTimeStamp &time ) const
  {
    return stamp > time;           
  };

private:
  CTimeStamp stamp;
  int person1, person2;
};

class CEFaceMask
{
public:
  CEFaceMask() {};

  CEFaceMask &addContact( const CContact &newContact )
  {
    if( newContact.hasDistinctPeople() ) contactList.push_back( newContact );
    return *this;
  };

  std::vector<int> listContacts( const int person ) const
  {
    std::vector<int> personContacts;
    for( const CContact &contact: contactList )
    {
      if( !contact.contain( person ) ) continue;
      int newContactPerson = contact.other( person );
      if( std::none_of( personContacts.begin(),
                        personContacts.end(),
                        [ newContactPerson ]( const int past )
                        {
                          return past == newContactPerson;
                        } ) )
      {
        personContacts.push_back( newContactPerson );
      };
    }
    return personContacts;
  };

  /* <from, to >  */
  std::vector<int> listContacts( const int person, const CTimeStamp &from, const CTimeStamp &to ) const
  {
    std::vector<int> personContacts;
    for( const CContact &contact: contactList )
    {
      if( !contact.contain( person ) ) continue;
      if( contact < from || contact > to ) continue;
      int newContactPerson = contact.other( person );
      if( std::none_of( personContacts.begin(),
                        personContacts.end(),
                        [ newContactPerson ]( const int past )
                        {
                          return past == newContactPerson;
                        } ) )
      {
        personContacts.push_back( newContactPerson );
      };
    }
    return personContacts;
  };

private:
  std::vector< CContact > contactList;
};

#ifndef __PROGTEST__
int main ()
{
  CEFaceMask test;

  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 10, 12, 40, 10 ), 123456789, 999888777 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 12, 12, 40, 10 ), 123456789, 111222333 ) )
       . addContact ( CContact ( CTimeStamp ( 2021, 2, 5, 15, 30, 28 ), 999888777, 555000222 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 2, 21, 18, 0, 0 ), 123456789, 999888777 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 1, 5, 18, 0, 0 ), 123456789, 456456456 ) );
  test . addContact ( CContact ( CTimeStamp ( 2021, 2, 1, 0, 0, 0 ), 123456789, 123456789 ) );
  assert ( test . listContacts ( 123456789 ) == (std::vector<int> {999888777, 111222333, 456456456}) );
  assert ( test . listContacts ( 999888777 ) == (std::vector<int> {123456789, 555000222}) );
  assert ( test . listContacts ( 191919191 ) == (std::vector<int> {}) );
  assert ( test . listContacts ( 123456789, CTimeStamp ( 2021, 1, 5, 18, 0, 0 ), CTimeStamp ( 2021, 2, 21, 18, 0, 0 ) ) == (std::vector<int> {999888777, 111222333, 456456456}) );
  assert ( test . listContacts ( 123456789, CTimeStamp ( 2021, 1, 5, 18, 0, 1 ), CTimeStamp ( 2021, 2, 21, 17, 59, 59 ) ) == (std::vector<int> {999888777, 111222333}) );
  assert ( test . listContacts ( 123456789, CTimeStamp ( 2021, 1, 10, 12, 41, 9 ), CTimeStamp ( 2021, 2, 21, 17, 59, 59 ) ) == (std::vector<int> {111222333}) );
  return 0;
}
#endif /* __PROGTEST__ */
