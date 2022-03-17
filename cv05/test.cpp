#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

class CTimeTester;
#endif /* __PROGTEST__ */

class CTime
{
private:
    int m_Hour;
    int m_Minute;
    int m_Second;

    static constexpr int DAY = 24 * 60 * 60;

    static CTime fromSeconds( int seconds )
    {
        seconds = ( seconds % DAY + DAY ) % DAY;
        return CTime( seconds / 3600,
                      seconds / 60 % 60,
                      seconds % 60 );
    };

    int toSeconds() const
    {
        return m_Hour * 3600 + m_Minute * 60 + m_Second;
    };

    static int timeDiff( const CTime &lhs, const CTime &rhs )
    {
        return lhs.toSeconds() - rhs.toSeconds();
    };

    static int reduce( int time )
    {
        return time % DAY;
    };

public:
    // constructor, destructor
    CTime()
        : m_Hour( 0 ), m_Minute( 0 ), m_Second( 0 )
    {};
    
    CTime( int hour, int minute, int second = 0 )
        : m_Hour( hour ), m_Minute( minute ), m_Second( second )
    {
        if( hour < 0 || hour > 23 ) throw std::invalid_argument( "Invalid hour" );
        if( minute < 0 || minute > 59 ) throw std::invalid_argument( "Invalid minute" );
        if( second < 0 || second > 59 ) throw std::invalid_argument( "Invalid second" );
    };

    // arithmetic operators

    friend CTime operator+( const CTime &lhs, int dt )
    {
        return CTime::fromSeconds( lhs.toSeconds() + reduce( dt ) );
    };

    friend CTime operator+( int dt, const CTime &rhs )
    {
        return rhs + dt;
    };

    CTime &operator+=( int dt )
    {
        return *this = *this + dt;
    };

    friend CTime operator-( const CTime &lhs, int dt )
    {
        return lhs + ( -reduce( dt ) );
    };

    CTime &operator-=( int dt )
    {
        return *this = *this - dt;
    };

    friend int operator-( const CTime &lhs, const CTime &rhs )
    {
        return std::min( std::abs( CTime::timeDiff( lhs, rhs ) ),
                         std::abs( CTime::timeDiff( rhs, lhs ) ) );
    };

    CTime operator++( int )
    {
        CTime temp( *this );
        *this += 1;
        return temp;
    };

    CTime &operator++( void )
    {
        return *this += 1;
    };

    CTime operator--( int )
    {
        CTime temp( *this );
        *this -= 1;
        return temp;
    };

    CTime &operator--( void )
    {
        return *this -= 1;
    };

    // comparison operators
    friend bool operator<( const CTime &lhs, const CTime &rhs )
    {
        return lhs.toSeconds() < rhs.toSeconds();
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

    friend bool operator==( const CTime &lhs, const CTime &rhs  )
    {
        return !( lhs > rhs ) && !( lhs < rhs );
    };

    friend bool operator!=( const CTime &lhs, const CTime &rhs  )
    {
        return !( lhs == rhs );
    };

    // output operator
    friend std::ostream &operator<<( std::ostream &os, const CTime &time )
    {
        std::ostream::char_type fillChar = os.fill();
        return os << std::setfill( ' ' )
                  << std::setw( 2 ) << time.m_Hour << ':'
                  << std::setfill( '0' )
                  << std::setw( 2 ) << time.m_Minute << ':'
                  << std::setfill( '0' )
                  << std::setw( 2 ) << time.m_Second
                  << std::setfill( fillChar );
    }


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
