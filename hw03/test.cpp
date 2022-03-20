#include "test.hpp"

const int CDate::monthLengths[] =
{
  0,
  31,
  28,
  31,
  30,
  31,
  30,
  31,
  31,
  30,
  31,
  30,
  31,
};

const int CDate::daysTillMonth[] = 
{
  0,   //0
  0,   //1
  31,  //2
  59,  //3
  90,  //4
  120, //5
  151, //6
  181, //7
  212, //8
  243, //9
  273, //10
  304, //11
  334  //12
};

//=================================================================================================
// a dummy exception class, keep this implementation
InvalidDateException::InvalidDateException ( )
  : invalid_argument ( "invalid date or format" )
{}

//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base & ( * date_format ( const char * fmt ) ) ( ios_base & x )
{
  return [] ( ios_base & ios ) -> ios_base & { return ios; };
}

//=================================================================================================
CDate::CDate()
  : days( 0 )
{}

CDate::CDate( int year, int month, int day )
{
  if( year < 2000 || year > 2030 ||
      month < 1 || month > 12 ||
      day < 1 ||
      day > monthLengths[ month ] + ( isLeapYear( year ) && month == 2 ) )
    throw InvalidDateException();
  
  days = day - 1;
  days += daysTillMonth[ month ];
  days += ( year - 2000 ) * 365;
  days += leapYearCount( year ) - leapYearCount( 2000 );
  days += isLeapYear( year ) && month > 2;
}

CDate operator+( const CDate &lhs, const int rhs )
{
  CDate temp( lhs );
  temp.days += rhs;
  return temp;
}

CDate operator+( const int lhs, const CDate &rhs )
{
  return rhs + lhs;
}

CDate operator-( const CDate &lhs, const int rhs )
{
  return lhs + (-rhs);
}

int operator-( const CDate &lhs, const CDate &rhs )
{
  return lhs.days - rhs.days;
}

CDate CDate::operator++( int )
{
  CDate temp( *this );
  *this += 1;
  return temp;
}

CDate &CDate::operator++( void )
{
  return *this += 1;
}

CDate CDate::operator--( int )
{
  CDate temp( *this );
  *this -= 1;
  return temp;
}

CDate &CDate::operator--( void )
{
  return *this -= 1;
}

CDate &CDate::operator+=( const int dt )
{
  return *this = *this + dt;
}

CDate &CDate::operator-=( const int dt )
{
  return *this = *this - dt;
}

bool operator<( const CDate &lhs, const CDate &rhs )
{
  return lhs.days < rhs.days;
}

bool operator>( const CDate &lhs, const CDate &rhs )
{
  return rhs < lhs;
}

bool operator<=( const CDate &lhs, const CDate &rhs )
{
  return !( lhs > rhs );
}

bool operator>=( const CDate &lhs, const CDate &rhs )
{
  return !( lhs < rhs );
}

bool operator==( const CDate &lhs, const CDate &rhs )
{
  return lhs.days == rhs.days;
}

bool operator!=( const CDate &lhs, const CDate &rhs )
{
  return !( lhs == rhs );
}

ostream &operator<<( ostream &os, const CDate &timeOut )
{
  ostream::char_type fillChar = os.fill();
  ostream::fmtflags initial = os.flags();
  os << setfill( '0' ) << right
     << setw( 4 ) << timeOut.getYear() << '-'
     << setw( 2 ) << timeOut.getMonth() << '-'
     << setw( 2 ) << timeOut.getDay()
     << setfill( fillChar );
  os.flags( initial );
  return os;
}

istream &operator>>( istream &is, CDate &timeIn )
{
  int year, month, day;
  char tempChar;
  is >> year >> tempChar;
  if( tempChar != '-' )
  {
    is.setstate( ios::failbit );
    return is;
  }
  is >> month >> tempChar;
  if( tempChar != '-' )
  {
    is.setstate( ios::failbit );
    return is;
  }
  is >> day;

  if( !is )
  {
    is.setstate( ios::failbit );
    return is;
  }

  try
  {
    CDate output( year, month, day );
    timeIn = output;
  }
  catch( const InvalidDateException & )
  {
    is.setstate( ios::failbit );
  }
  return is;
}

int CDate::getYear() const
{
  int tempDays = days;
  int res = 4 * ( tempDays / ( 366 + 3 * 365 ) );
  tempDays %= 366 + 3 * 365;
  res += --tempDays / 365;
  return res + 2000;
}

int CDate::getMonth() const
{
  int tempDays = days;
  tempDays %= ( 366 + 3 * 365 );

  if( tempDays >= 366 ) tempDays = ( tempDays - 1 ) % 365;

  for( int month = 1;; ++month )
  {
    int monthLength = monthLengths[ month ] + ( isLeapYear( getYear() ) && month == 2 );
    if( tempDays >= monthLength )
      tempDays -= monthLength;
    else return month;
  }
}

int CDate::getDay() const
{
  int tempDays = days;
  tempDays %= ( 366 + 3 * 365 );

  if( tempDays >= 366 ) tempDays = ( tempDays - 1 ) % 365;

  for( int month = 1;; ++month )
  {
    int monthLength = monthLengths[ month ] + ( isLeapYear( getYear() ) && month == 2 );
    if( tempDays >= monthLength )
      tempDays -= monthLength;
    else return tempDays + 1;
  }
}

bool CDate::isLeapYear( const int year )
{
  return year % 4 == 0;
}

int CDate::leapYearCount( int year )
{
  return ( year - 1 ) / 4;
}
