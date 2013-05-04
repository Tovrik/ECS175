#ifndef _POINT_HPP_
#define _POINT_HPP_

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h> 
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/gl.h> 
#include <GL/glu.h>
#include <GL/glut.h>
#endif

class Point{
public:
	GLfloat x, y;

	Point(GLfloat my_x, GLfloat my_y){
		x = my_x;
		y = my_y;
	}

	Point(){}

	void setX(GLfloat my_x) {x = my_x;}
	void setY(GLfloat my_y) {y = my_y;}
	const Point & operator=(const Point &my_Point){
		x = my_Point.x;
		y = my_Point.y;
		return *this;
	}
};

#endif