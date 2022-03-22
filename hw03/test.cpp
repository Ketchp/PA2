#include "test.hpp"

static size_t strlen( const char *str );
static int toInt( const char *str );

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

_dateFormat::_dateFormat( const char *format )
  : _format( format )
{}

void _dateFormat::event_callback( ios_base::event type, ios_base& ios, int index )
{
  void * &state_ptr = ios.pword( index );
  if( type == ios_base::event::erase_event )
  {
    delete (string *)state_ptr;
  }
  else if( type == ios_base::event::copyfmt_event )
  {
    state_ptr = ( void * ) new string( *(string *)state_ptr );
  }
}

ostream &operator<<( ostream &os, _dateFormat &&f_obj )
{
  void * &state_ptr = os.pword( CDate::base_ios_index );

  if( state_ptr )
    // delete old format
    delete (string *)state_ptr;
  else
    os.register_callback( &_dateFormat::event_callback, CDate::base_ios_index );

  // move format from temporary object
  state_ptr = (void *) new string( f_obj._format ); // TODO ask cviciaci
  
  return os; 
}

istream &operator>>( istream &is, _dateFormat &&f_obj )
{
  void * &state_ptr = is.pword( CDate::base_ios_index );
  
  if( state_ptr )
    // delete old format
    delete (string *)state_ptr;
  else
    is.register_callback( &_dateFormat::event_callback, CDate::base_ios_index );

  // move format from temporary object
  state_ptr = (void *) new string( f_obj._format ); // TODO ask cviciaci
  
  return is;
}

//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
_dateFormat date_format( const char * fmt )
{
  return _dateFormat( fmt );
}

//=================================================================================================
const int CDate::base_ios_index = ios_base::xalloc();

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
  void * &state_ptr = os.pword( CDate::base_ios_index );
  const string &format = state_ptr ? *(string *)state_ptr : "%Y-%m-%d";

  ostream::char_type fillChar = os.fill();
  ostream::fmtflags initial = os.flags();

  os << setfill( '0' ) << right;
  auto it = format.cbegin();
  while( it != format.cend() )
  {
    if( *it != '%' )
    {
      os.put( *it );
      ++it;
      continue;
    }
    ++it;
    if( it == format.cend() )
    {
      os.setstate( ios_base::failbit );
      break;
    }
    if( *it == 'Y' )
      os << setw( 4 ) << timeOut.getYear();
    else if( *it == 'm' )
      os << setw( 2 ) << timeOut.getMonth();
    else if( *it == 'd' )
      os << setw( 2 ) << timeOut.getDay();
    else
      os.put( *it );

    ++it;
  }
  os.fill( fillChar );
  os.flags( initial );
  return os;
}

istream &operator>>( istream &is, CDate &timeIn )
{
  void * &state_ptr = is.pword( CDate::base_ios_index );
  const string &format = state_ptr ? *(string *)state_ptr : "%Y-%m-%d";

  int year = 0, month = 0, day = 0;
  char tempChar;
  char buffer[ 5 ];

  auto it = format.cbegin();
  while( it != format.cend() )
  {
    if( *it != '%' )
    {
      is.get( tempChar );
      if( tempChar != *it )
      {
        year = 0;
        break;
      }
      ++it;
      continue;
    }
    ++it;
    if( it == format.cend() )
    {
      year = 0;
      break;
    }
    if( *it == 'Y' )
    {
      is.get( buffer, 5 );
      if( strlen( buffer ) != 4 || year )
      {
        year = 0;
        break;
      }
      if( !( year = toInt( buffer ) ) ) break;
    }
    else if( *it == 'm' )
    {
      is.get( buffer, 3 );
      if( strlen( buffer ) != 2 || month )
      {
        month = 0;
        break;
      }
      if( !( month = toInt( buffer ) ) ) break;
    }
    else if( *it == 'd' )
    {
      is.get( buffer, 3 );
      if( strlen( buffer ) != 2 || day )
      {
        day = 0;
        break;
      }
      if( !( day = toInt( buffer ) ) ) break;
    }
    else if ( !is.get( tempChar ) || tempChar != *it )
    {
      year = 0;
      break;
    }
    ++it;
  }

  if( !day || !month || !year )
  {
    is.setstate( ios_base::failbit );
    return is;
  }

  try
  {
    timeIn = CDate( year, month, day );
    return is;
  }
  catch( const InvalidDateException &e )
  {
    is.setstate( ios_base::failbit );
    return is;
  }
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

static size_t strlen( const char *str )
{
  size_t size = 0;
  while( *str++ )
    ++size;
  return size;
}

static int toInt( const char *str )
{
  int num = 0;
  while( *str )
  {
    if( *str < '0' || '9' < *str ) return 0;
    num = 10 * num + ( *str - '0' );
    str++;
  }
  return num;
}
