#include "CList.hpp"

CList::CList ( void )
:m_used(0),
 m_capacity(0),
 m_data(nullptr)
{}

CList::~CList ( void )
{
    delete [] m_data;
}

CList & CList::add_student ( const SStudent & student )
{
    if(m_used >= m_capacity)
    {
        expand();
    }
    m_data[m_used++] = student;
    return *this;
}

const SStudent & CList::get_student ( size_t idx ) const
{
    return m_data[idx];
}

size_t CList::size ( void ) const
{
    return m_used;
}

CList::const_iterator CList::begin ( void ) const
{
    return m_data;
}

CList::const_iterator CList::end ( void ) const
{
    return begin() + size();
}

CList::iterator CList::begin ( void )
{
    return m_data;
}

CList::iterator CList::end ( void )
{
    return begin() + size();
}

void CList::expand ( void )
{
    size_t new_capacity = m_capacity * 2 + 1;
    SStudent * new_data = new SStudent[new_capacity];
    for(size_t i = 0; i < m_used; ++i)
    {
        new_data[i] = m_data[i];
    }
    delete [] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

