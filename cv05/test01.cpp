#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
using namespace std;

class CTimeTester;
#endif /* __PROGTEST__ */

class CTime
{
private:
    int m_Hour;
    int m_Minute;
    int m_Second;

public:
    // constructor, destructor
    CTime( int hour, int minute, int second = 0 )
        : m_Hour( hour ), m_Minute( minute ), m_Second( second )
    {
        if( hour < 0 || 23 < hour ) throw std::invalid_argument( "Invalid hour" );
        if( minute < 0 || minute > 59 ) throw std::invalid_argument( "Invalid minute" );
        if( second < 0 || second > 59 ) throw std::invalid_argument( "Invalid second" );
    };

    explicit CTime( int seconds = 0 )
        : m_Hour( ( ( seconds / 3600 % 24 ) + 24 ) % 24 ),
          m_Minute( ( ( seconds / 60 % 60 ) + 60 ) % 60 ),
          m_Second( ( ( seconds % 60 ) + 60 ) % 60 )
    {};

    int seconds() const
    {
        return m_Hour * 3600 + m_Minute * 60 + m_Second;
    }

    // arithmetic operators
    friend CTime operator+( const CTime &lhs, int dt )
    {
        return CTime( lhs.m_Hour * 3600 + lhs.m_Minute * 60 + lhs.m_Second + dt % ( 24 * 3600 ) );
    };

    friend CTime operator+( int dt, const CTime &lhs )
    {
        return lhs + dt;
    };

    CTime &operator+=( int dt )
    {
        return *this = *this + dt;
    };

    friend CTime operator-( const CTime &lhs, int dt )
    {
        return CTime( lhs.seconds() - dt );
    };

    friend int operator-( const CTime &lhs, const CTime &rhs )
    {
        return ( lhs.seconds() - rhs.seconds() + 3600 * 24 ) % ( 3600 * 24 );
    };

    CTime operator-() const
    {
        return CTime( 60 * 60 * 24 - this->seconds() );
    };

    CTime &operator-=( const CTime &other )
    {
        return *this = CTime( *this - other );
    };

    CTime &operator-=( int dt )
    {
        return *this = *this - dt;
    };

    CTime operator++( int )
    {
        CTime temp( *this );
        *this += 1;
        return temp;
    };

    CTime &operator++()
    {
        return *this += 1;
    };

    CTime operator--( int )
    {
        CTime temp( *this );
        *this -= 1;
        return temp;
    };

    CTime &operator--()
    {
        return *this -= 1;
    };


    // comparison operators
    friend bool operator<( const CTime &lhs, const CTime &rhs )
    {
        return lhs.seconds() < rhs.seconds();
    };

    friend bool operator>( const CTime &lhs, const CTime &rhs )
    {
        return rhs < lhs;
    };

    friend bool operator<=( const CTime &lhs, const CTime &rhs )
    {
        return !( lhs > rhs );
    };

    friend bool operator>=( const CTime &lhs, const CTime &rhs )
    {
        return !( lhs < rhs );
    };

    friend bool operator==( const CTime &lhs, const CTime &rhs )
    {
        return !( lhs < rhs ) && !( lhs > rhs );
    };

    friend bool operator!=( const CTime &lhs, const CTime &rhs )
    {
        return !( lhs == rhs );
    };

    // output operator
    friend ostream &operator<<( ostream &os, const CTime &time )
    {
        ostream::char_type fill = os.fill();
        return os << std::setfill( ' ' )
                  << std::setw( 2 ) << time.m_Hour << ':'
                  << std::setfill( '0' )
                  << std::setw( 2 ) << time.m_Minute << ':'
                  << std::setw( 2 ) << time.m_Second
                  << std::setfill( fill );
    };

    friend class ::CTimeTester;
};

#ifndef __PROGTEST__
struct CTimeTester
{
    static bool test ( const CTime & time, int hour, int minute, int second )
    {
        return time.m_Hour == hour 
            && time.m_Minute == minute
            && time.m_Second == second;
    }
};

#include <cassert>
#include <sstream>

int main ()
{
    CTime a { 12, 30 };
    assert( CTimeTester::test( a, 12, 30, 0 ) );

    CTime b { 13, 30 };
    assert( CTimeTester::test( b, 13, 30, 0 ) );

    assert( b - a == 3600 );

    assert( CTimeTester::test( a + 60, 12, 31, 0 ) );
    assert( CTimeTester::test( a - 60, 12, 29, 0 ) );

    assert( CTimeTester::test( a +     1, 12, 30, 1 ) );
    assert( CTimeTester::test( a +    61, 12, 31, 1 ) );
    assert( CTimeTester::test( a +  3600, 13, 30, 0 ) );
    assert( CTimeTester::test( a +  3599, 13, 29, 59 ) );
    assert( CTimeTester::test( a + 12 * 3600, 0, 30, 0 ) );
    assert( CTimeTester::test( a + 12 * 3600 + 25*60 + 35, 0, 55, 35 ) );
    assert( CTimeTester::test( a + 1654464, 16, 4, 24 ) );
    assert( CTimeTester::test( a - 1, 12, 29, 59 ) );


    assert( a < b );
    assert( a <= b );
    assert( a != b );
    assert( !( a > b ) );
    assert( !( a >= b ) );
    assert( !( a == b ) );

    while ( ++a != b );
    assert( a == b );

    std::ostringstream output;
    assert( static_cast<bool>( output << a ) );
    assert( output.str() == "13:30:00" );

    assert( a++ == b++ );
    assert( a == b );

    assert( --a == --b );
    assert( a == b );

    assert( a-- == b-- );
    assert( a == b );

    return 0;
}
#endif /* __PROGTEST__ */
