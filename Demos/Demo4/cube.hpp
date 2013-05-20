/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 4
 
 */
#ifndef _CUBE_HPP_
#define _CUBE_HPP_


/*
 Cube class (singleton)
 */
class Cube
{
private:
    static unsigned int m_vertexbuffer;     //points to vertex buffer object
    Cube(const int pos_attribute_location, const int normal_attribute_location);
    ~Cube();
public:
    static Cube& init(const int pos_attribute_location,const int normal_attribute_location);
    static void drawCube();
};




#endif

