/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 5
 
 */
#ifndef _CURVE_HPP_
#define _CURVE_HPP_


/*
 Curve class
 */
class Circle
{
private:
    int sections;                   //number of sections used to discretize circle
    unsigned int m_vertexbuffer;     //points to vertex buffer object
   
public:
    Circle(const float mx, const float my, const float radius, const int sections);
    ~Circle();
    void draw();
};




#endif

