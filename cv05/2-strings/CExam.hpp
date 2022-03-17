#pragma once

#include "CTeacher.hpp"
#include "CList.hpp"

class CExam
{
	public:
		CExam ( CTeacher & teacher, int difficulty_level );
		void start_exam ( const CList & student_list ) const;
	private:
		CTeacher & m_teacher;
        int m_difficulty_level;
};
