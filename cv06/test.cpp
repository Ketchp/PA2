#ifndef __PROGTEST__
#include <cstring>
using namespace std;

class CLinkedSetTester;
#endif /* __PROGTEST__ */


class CLinkedSet
{
private:
    struct CNode
    {
        CNode()
            : m_Next( nullptr ), m_Value( nullptr )
        {};

        CNode( const char *val, CNode *next = nullptr )
            : m_Next( next )
        {
            size_t length = strlen( val );
            m_Value = new char[ length + 1 ];
            strcpy( m_Value, val );
        };

        CNode( const CNode &sample )
            : m_Next( nullptr ), m_Value( nullptr )
        {
            if( sample.m_Value )
            {
                size_t length = strlen( sample.m_Value );
                m_Value = new char[ length + 1 ];
                strcpy( m_Value, sample.m_Value );
            }
            if( !sample.m_Next )
                return;
            m_Next = new CNode( *sample.m_Next );                
        };

        ~CNode()
        {
            delete[] m_Value;
            m_Value = nullptr;
            delete m_Next;
            m_Next = nullptr;
        };

        CNode *m_Next;
        char *m_Value;

        const char *Value() const
        {
            return m_Value;
        };
    };

    CNode *m_Begin;
    size_t m_size;

public:
    // default constructor
    CLinkedSet()
        : m_Begin( nullptr ), m_size( 0 )
    {};

    // copy constructor
    CLinkedSet( const CLinkedSet &sample )
        : m_Begin( nullptr ), m_size( 0 )
    {
        *this = sample;
    };

    // operator=
    CLinkedSet &operator=( const CLinkedSet &rhs )
    {
        if( this == &rhs )
            return *this;
        delete m_Begin;
        m_Begin = nullptr;
        m_size = rhs.m_size;
        if( rhs.m_Begin )
            m_Begin = new CNode( *rhs.m_Begin );
        return *this;
    }

    // destructor
    ~CLinkedSet()
    {
        delete m_Begin;
        m_Begin = nullptr;
    }

    bool Insert( const char *value )
    {
        if( !m_Begin )
        {
            m_Begin = new CNode( value );
            ++m_size;
            return true;
        }
        if( !strcmp( m_Begin->m_Value, value ) )
            return false;
        if( strcmp( m_Begin->m_Value, value ) > 0 )
        {
            m_Begin = new CNode( value, m_Begin );
            ++m_size;
            return true;
        }        
        CNode *current = m_Begin;
        while( current->m_Next )
        {
            int comp = strcmp( current->m_Next->m_Value, value );
            if( !comp )
                return false;
            if( comp > 0 )
                break;
            current = current->m_Next;
        }

        current->m_Next = new CNode( value, current->m_Next );
        ++m_size;
        return true;
    };
    
    bool Remove( const char *value )
    {
        if( !m_Begin )
            return false;
        if( !strcmp( m_Begin->m_Value, value ) )
        {
            CNode *temp = m_Begin->m_Next;
            m_Begin->m_Next = nullptr;
            delete m_Begin;
            m_Begin = temp;
            --m_size;
            return true;
        }
        CNode *current = m_Begin;
        while( current->m_Next )
        {
            int comp = strcmp( current->m_Next->m_Value, value );
            if( !comp )
            {
                CNode *temp = current->m_Next->m_Next;
                current->m_Next->m_Next = nullptr;
                delete current->m_Next;
                current->m_Next = temp;
                --m_size;
                return true;
            }
            if( comp > 0 )
                return false;
            current = current->m_Next;
        }
        return false;
    };

    bool Empty() const
    {
        return !m_size;
    };

    size_t Size() const
    {
        return m_size;
    };

    bool Contains( const char *value ) const
    {
        CNode *current = m_Begin;
        while( current )
        {
            int comp = strcmp( current->m_Value, value );
            if( !comp )
                return true;
            if( comp > 0 )
                break;
            current = current->m_Next;
        }
        return false;
    };

    friend class::CLinkedSetTester;
};

#ifndef __PROGTEST__

#include <cassert>

struct CLinkedSetTester
{
    static void test0()
    {
        CLinkedSet x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        assert( !x0 . Empty() );
    };

    static void test1()
    {
        CLinkedSet x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        CLinkedSet x1 ( x0 );
        assert( x0 . Insert( "Vodickova Saskie" ) );
        assert( x1 . Insert( "Kadlecova Kvetslava" ) );
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
        assert( x0 . Contains( "Vodickova Saskie" ) );
        assert( !x1 . Contains( "Vodickova Saskie" ) );
        assert( !x0 . Contains( "Kadlecova Kvetslava" ) );
        assert( x1 . Contains( "Kadlecova Kvetslava" ) );
    };

    static void test2()
    {
        CLinkedSet x0;
        CLinkedSet x1;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        x1 = x0;
        assert( x0 . Insert( "Vodickova Saskie" ) );
        assert( x1 . Insert( "Kadlecova Kvetslava" ) );
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
        assert( x0 . Contains( "Vodickova Saskie" ) );
        assert( !x1 . Contains( "Vodickova Saskie" ) );
        assert( !x0 . Contains( "Kadlecova Kvetslava" ) );
        assert( x1 . Contains( "Kadlecova Kvetslava" ) );
    };

    static void test3()
    {
        CLinkedSet x0;
        assert( x0 . Insert( "B" ) );
        assert( x0 . Insert( "C" ) );
        assert( x0 . Insert( "E" ) );
        assert( x0 . Insert( "D" ) );
        assert( x0 . Insert( "F" ) );
        assert( x0 . Insert( "A" ) );
    };

    static void test4()
    {
        CLinkedSet x0, x1, x2, x3;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x1 . Insert( "Pavlik Ales" ) );
        assert( x1 . Insert( "Dusek Zikmund" ) );
        x1 = x0;
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
    };
};

int main ()
{
    CLinkedSetTester::test0();
    CLinkedSetTester::test1();
    CLinkedSetTester::test2();
    CLinkedSetTester::test3();
    CLinkedSetTester::test4();
    return 0;
}

#endif /* __PROGTEST__ */

/*

void initialize( complex *c )
{
    c->a = malloc( sizeof( int ) );
    c->b = malloc( sizeof( int ) );
}

void destruct( complex *c )
{
    free( c->a );
    free( c->b )
}

struct complex
{
    complex()
    {
        a = new int;
        b = new int;
    }
    ~complex()
    {
        delete a;
        delete b;
    }
    int *a, *b;
}

int main()
{
    int a = 5, b = 6;
    {
    complex x;
    initialize( &x );

    //smth with x;

    destruct( &x );
    }
}

*/