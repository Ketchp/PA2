#include "fraction.hpp"
#include <sstream>
#include <cassert>
#include <iostream>

int main ( void )
{
	CFraction a1;
	assert (  a1 == CFraction(0,1) );
	assert ( -a1 == CFraction(0,1) );
	assert ( CFraction(-1,-1) == CFraction(1,1) );
	assert ( CFraction(15,3) == CFraction(5,1) );
	assert ( CFraction(5,-3) == CFraction(-5,3) );
	
	a1 = a1 + 10;
	assert ( a1 == CFraction(10,1) );

	a1 = a1 / 5;
	assert ( a1 == CFraction(2,1) );

	a1 = a1 / 5;
	assert ( a1 == CFraction(2,5) );

	a1 *= (-3);
	assert ( a1 == CFraction(-6,5) );

	a1 /= 2;
	assert ( a1 == CFraction(-3,5) );

	a1 /= -3;
	assert ( a1 == CFraction(1,5) );

	a1 += 0;
	assert ( a1 == CFraction(1,5) );
	
	a1 += -2;
	assert ( a1 == CFraction(-9,5) );

	a1 -= -2;
	assert ( a1 == CFraction(1,5) );

	a1 += 2;
	assert ( a1 == CFraction(11,5) );

	a1 -= 2;
	assert ( a1 == CFraction(1,5) );

	CFraction a2 = a1 + 1;
	assert ( a1 != a2 );
	assert ( a2 >  a1 );
	assert ( a2 >= a1 );
	assert ( a1 <  a2 );
	assert ( a1 <= a2 );
	
	a1++;
	assert (   a1 == a2  );
	assert ( !(a2 <  a1) );
	assert ( !(a2 >  a1) );
	assert ( !(a2 != a1) );
	assert (   a2 <= a1  );
	assert (   a2 >= a1  );

	assert ( a1++ == a2 );
	assert ( a1 == ++a2 );
	assert ( a1 == a2 );
	
	assert ( a1.operator++(10) == a2 );
	a2 += 10;
	assert ( a1 == a2 );
	
	assert ( a1.operator--(4) == a2-- );
	assert ( a1 < a2.operator--(2) );
	assert ( a1 == --a2 );

	a2 = a1;
	a2 *= 2;

	assert ( a1 * 2 == a2 );
	assert ( a1 == a2 / 2 );

	assert ( -a1 <   a2 );
	assert ( -a1 <=  a2 );
	assert ( -a1 !=  a2 );
	assert (  a2 >  -a1 );
	assert (  a2 >= -a1 );
	assert (  a2 != -a1 );
	assert (  a1 - 3 < a2 );
	assert (  a1 < a2 + 3 );
	assert (  a1 < a2 - 2 );

	assert ( -a1 * a2 <  0 );
	assert ( -a1 * a2 <= 0 );

	std::ostringstream oss;
	oss << a1;
	assert ( oss.str() == "41/5" );

	oss.str("");
	oss << (-a1);
	assert ( oss.str() == "-41/5" );

	a1 -= CFraction(1,5);
	oss.str("");
	oss << a1;
	assert ( oss.str() == "8/1" );

	a1 += -8;
	oss.str("");
	oss << (-a1);
	assert ( oss.str() == "0/1" );

	a1 *= 0;
	assert ( a1 == CFraction(0,1) );

	std::istringstream iss;
	iss.str("5/2");
	iss >> a1;
	assert ( iss && a1 == CFraction(5,2) );

	iss.clear();
	iss.str("24/48");
	iss >> a1;
	assert ( ! iss.fail() );
	assert ( a1 == CFraction(1,2) );

	iss.clear();
	iss.str("24/0");
	iss >> a1;
	assert ( iss.fail() );

	iss.clear();
	iss.str("0/5");
	iss >> a1;	
	assert ( ! iss.fail() );
	assert ( a1 == CFraction(0,1) );

	iss.clear();
	iss.str("-3/8");
	iss >> a1;
	assert ( ! iss.fail() );
	assert ( a1 == CFraction(-3,8) );

	assert ( 5 - CFraction(3) == 2  );
	assert ( 5 + CFraction(3) == 8  );
	assert ( 5 * CFraction(3) == 15 );
	assert ( 6 / CFraction(3) == 2  );
	return 0;
}
