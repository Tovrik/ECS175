/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 4
 
*/

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


#include "cube.hpp"
#include <iostream>

unsigned int Cube::m_vertexbuffer;

/*
 Construct cube - initialize vertex buffer object.
 
 Buffers replace old obsolete glBegin/glEnd fixed function pipeline drawing techniques.
 */
Cube::Cube(const int pos_attribute_location, const int normal_attribute_location)
{
    //vertex and normal data for VBO
    //this array holds position values (first half) of vertices and normal data for vertices (second half)
    float data[] = {
    //vertices
    //Bottom face
    0,0,0,
    1,0,0,
    1,0,1,
    0,0,0,
    1,0,1,
    0,0,1,
	
    // Top 
    0,1,0,
    0,1,1,
    1,1,1,
    0,1,0,
    1,1,1,
    1,1,0,
	
    // Front 
    0,0,1,
    1,0,1,
    1,1,1,
    0,0,1,
    1,1,1,
    0,1,1,
	
    // Back 
    0,0,0,
    0,1,0,
    1,1,0,
    0,0,0,
    1,1,0,
    1,0,0,
    
    // Right 
    1,0,0,
    1,1,0,
    1,1,1,
    1,0,0,
    1,1,1,
    1,0,1,
	
    // Left 
    0,0,0,
    0,0,1,
    0,1,1,
    0,0,0,
    0,1,1,
    0,1,0,
        
    //normals
    //bottom face
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,
        
    // Top 
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
        
    // Front 
    0,0,1,
    0,0,1,
    0,0,1,
    0,0,1,
    0,0,1,
    0,0,1,
        
    // Back 
    0,0,-1,
    0,0,-1,
    0,0,-1,
    0,0,-1,
    0,0,-1,
    0,0,-1,
    
    // Right 
    1,0,0,
    1,0,0,
    1,0,0,
    1,0,0,
    1,0,0,
    1,0,0,
        
    // Left 
    -1,0,0,
    -1,0,0,
    -1,0,0,
    -1,0,0,
    -1,0,0,
    -1,0,0};
    
       
    glGenBuffers(1, &m_vertexbuffer);               //generate a vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);  //bind buffer to be active
    glBufferData(GL_ARRAY_BUFFER, 216*sizeof(float), data, GL_STATIC_DRAW); //set buffer data
   
    
    glEnableVertexAttribArray(pos_attribute_location); //point to position attribute in shader
    glVertexAttribPointer(pos_attribute_location, 3, GL_FLOAT, GL_FALSE, 0, 0); //indicates array data of position attribute
   
    glEnableVertexAttribArray(normal_attribute_location); //point to normal attribute in shader
    glVertexAttribPointer(normal_attribute_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)(108*sizeof(float))); //indicates array data of normal attribute in shader
   
    
}

/*
 Destroy Cube - clean up geometry
 */
Cube::~Cube()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);       //unbind current buffer
    glDeleteBuffers(1, &m_vertexbuffer);    //delete buffer data
}


/*
 Draw Cube
 */
void Cube::drawCube()
{
    //note: if different VBOs are used, we have to bind the appropriate buffer and attribute pointers here
    
    glDrawArrays(GL_TRIANGLES, 0, 36);             //draw cube (tell OpenGL that buffer keeps triangles)
}

/*
 Singleton constructor
 Pointers to attribute locations in vertex shader are passed as argument for VBO setup in constructor.
 */
Cube& Cube::init(const int pos_attribute_location, const int normal_attribute_location)
{
    static Cube c(pos_attribute_location,normal_attribute_location);
    return c;
}