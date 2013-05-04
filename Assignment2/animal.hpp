/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Stefan Peterson
 SID#: 998556530 
 Assignment 2
 
*/

#ifndef _ANIMAL_HPP_
#define _ANIMAL_HPP_

#include "point.hpp"
 /*
	Animal Class
 */
class Animal{
private:
	int sections;
	unsigned int m_vertexbuffer;

public:
	Animal();
	~Animal();
	void drawDot(int x, int y);
	void drawLine(Point p1, Point p2);
	Point findBezierPoint(Point p1, Point p2, Point p3, Point p4, float t);
	void draw();
};

#endif