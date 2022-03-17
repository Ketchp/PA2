#include "CTeacher.hpp"

CTeacher::CTeacher ( const my_std::string & name )
:m_name(name),
 m_passed_students(0)
{}

size_t CTeacher::passed_students ( void ) const
{
    return m_passed_students;
}

void CTeacher::passed_students ( size_t newly_passed )
{
    m_passed_students += newly_passed;
}

