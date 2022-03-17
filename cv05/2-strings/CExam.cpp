#include "CExam.hpp"

CExam::CExam ( CTeacher & teacher, int difficulty_level )
:m_teacher(teacher),
 m_difficulty_level(difficulty_level)
{}

void CExam::start_exam ( const CList & student_list ) const
{
    size_t counter = 0;
    for(auto & student : student_list) {
        counter += student.m_knowledge_level >= m_difficulty_level;
    }
    m_teacher.passed_students(counter);
}

