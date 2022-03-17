#include <cassert>
#include "CTeacher.hpp"
#include "CExam.hpp"


int main ( void )
{
	CTeacher honza("Honza"), franta("Franta");
	assert( honza.passed_students() == 0 );

	CList student_list1;
	student_list1.add_student({"Tomas", 5}).add_student({"Patrik", 24}).add_student({"Pepa", 25}).add_student({"Jirka", 88});

	CExam exam1(honza, 25);
	exam1.start_exam(student_list1);
	assert( honza.passed_students() == 2 );

	exam1.start_exam(student_list1);
	assert( honza.passed_students() == 4 );

	CExam exam2(honza, 50);
	exam2.start_exam(student_list1);
	assert( honza.passed_students() == 5 );

	CList student_list2;
	CExam exam3(franta, 0);
	exam3.start_exam(student_list2);
	assert( franta.passed_students() == 0 );

	exam3.start_exam(student_list1);
	assert( franta.passed_students() == 4 );

	student_list2.add_student({"Franta", 99});
	
	CExam exam4(honza, 100), exam5(franta, 99);
	exam4.start_exam(student_list2);
	assert ( honza.passed_students() == 5 );

	exam5.start_exam(student_list2);
	assert ( franta.passed_students() == 5 );

	exam3.start_exam(student_list1);
	assert ( franta.passed_students() == 9 );

	#if defined(COPY_ALLOWED) && COPY_ALLOWED == 1
		CList student_list3 = student_list1;
		student_list3 = student_list2;
		student_list3 = student_list3;
		CList student_list4 = std::move(student_list3);
		student_list3 = std::move(student_list4);
		student_list3 = std::move(student_list3);
	#endif
	return 0;
}
