#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>

struct CPoint
{
	???? m_x;
	???? m_y;
	// ====================================================================
};

class CCircle
{
	public:
		???? init ( CPoint, unsigned int )
		{

		}
		// ====================================================================
		???? change_center ( CPoint )
		{

		}
		// ====================================================================
		???? contains_point ( CPoint ) 
		{

		}
		// ====================================================================
	private:

};

struct CTriangle
{
	static bool valid_triangle ( CPoint, CPoint, CPoint )
	{
		return false;
	}
	// ====================================================================
};

int main ( void )
{
	std::vector<CPoint> points = {{5,5}, {3,3}, {1,1}, {-6,-3}, {-2,2},
					   			  {3,1}, {-3,3}, {3,-4}, {-6,-5}, {1,2}};
	CCircle circle;
	assert ( circle.init({15,15}, 5).change_center({-2,-2}).contains_point({-2,-2}) );

	assert ( ! circle.contains_point({15,15}) );

	size_t counter = 0;
	for ( CPoint & point : points )
		counter += circle.contains_point(point);
	
	assert ( counter == 5 );

	/*
	// Pro spusteni tohoto testu pouze odkomentujte tuto sekci
	auto const_test = [](const CCircle & circle, const std::vector<CPoint> & points){
		size_t counter = 0;
		for ( const CPoint & point : points )
			counter += circle.contains_point(point);
		return counter;
	};
	assert ( const_test(circle, points) == 5 );
	*/

	/*
	assert ( ! CTriangle::valid_triangle(points[0], points[1], points[2]) );
	assert ( ! CTriangle::valid_triangle(points[4], points[9], {-8,2})    );
	assert ( ! CTriangle::valid_triangle(points[1], points[5], {3,-5})    );
	assert ( ! CTriangle::valid_triangle(points[0], points[1], points[2]) );
	assert ( ! CTriangle::valid_triangle(points[0], points[0], points[0]) );
	assert ( ! CTriangle::valid_triangle(points[0], points[0], points[1]) );
	
	assert (   CTriangle::valid_triangle(points[0], points[2], points[4]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[4]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[3]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[5]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[6]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[7]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[8]) );
	assert (   CTriangle::valid_triangle(points[0], points[1], points[9]) );
	*/
}