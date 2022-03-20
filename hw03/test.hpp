#pragma once

#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

//=================================================================================================
// a dummy exception class, keep this implementation
class InvalidDateException : public invalid_argument
{
public:
  InvalidDateException();
};

//=================================================================================================
// date_format manipulator - a dummy implementation. Keep this code unless you implement your
// own working manipulator.
ios_base & ( * date_format ( const char * fmt ) ) ( ios_base & x );

//=================================================================================================
class CDate
{
public:
  CDate();

  CDate( int year, int month, int day );

  friend CDate operator+( const CDate &lhs, const int rhs );
  friend CDate operator+( const int lhs, const CDate &rhs );
  friend CDate operator-( const CDate &lhs, const int rhs );
  friend int operator-( const CDate &lhs, const CDate &rhs );
  CDate operator++( int );
  CDate &operator++( void );
  CDate operator--( int );
  CDate &operator--( void );
  CDate &operator+=( const int dt );
  CDate &operator-=( const int dt );
  friend bool operator<( const CDate &lhs, const CDate &rhs );
  friend bool operator>( const CDate &lhs, const CDate &rhs );
  friend bool operator<=( const CDate &lhs, const CDate &rhs );
  friend bool operator>=( const CDate &lhs, const CDate &rhs );
  friend bool operator==( const CDate &lhs, const CDate &rhs );
  friend bool operator!=( const CDate &lhs, const CDate &rhs );
  friend ostream &operator<<( ostream &os, const CDate &timeOut );
  friend istream &operator>>( istream &is, CDate &timeIn );
  int getYear() const;
  int getMonth() const;
  int getDay() const;

private:
  int days;
  const static int monthLengths[];
  const static int daysTillMonth[];
  static bool isLeapYear( const int year );
  static int leapYearCount( int year );
};
