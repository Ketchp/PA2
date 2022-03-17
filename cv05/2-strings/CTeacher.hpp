#pragma once

#include "string.hpp"

class CTeacher
{
	public:
		CTeacher ( const my_std::string & name );
		size_t passed_students ( void ) const;
		void passed_students ( size_t newly_passed );
	private:
		my_std::string m_name;
		size_t m_passed_students;
};
