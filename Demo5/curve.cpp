/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 5
 
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

#include "math.h"
#include "curve.hpp"
#include <iostream>



/*
 Construct circle - sample parametric form of circle and initialize vertex buffer object.
 
 Buffers (and vertex arrays) replace old obsolete glBegin/glEnd fixed function pipeline drawing techniques.
 
 */
Circle::Circle(const float mx, const float my, const float radius, const int sections)
{
    this->sections = sections;
    
    /*
        sampling the parametric form of the curve (circle) to create lines for rendering
    */
    
    //vertex data of circle (3 floats per position= 3*sections floats)
    float *data = new float[3*sections+3];      //we duplicate the first point to get a closed circle - this needs 3 additional floats
    
    float u = 0.0;                              //current evaluation parameter
    const float du = (2*M_PI)/(float)sections;  //change of curve parameter between two samples
    for(int iu = 0;iu<sections;++iu)            //create [sections] samples of the parametric circle
    {
        data[iu*3] = mx + radius*sin(u);        //x coordinate for current u parameter value
        data[iu*3+1] =  my + radius*cos(u);     //y coordinate for current u parameter value
        data[iu*3+2] = 0;                       //z coordinate = 0
        u += du;                                //go to parameter value of next sample point
    }
    
    //make first and last point identical to get closed circle
    data[sections*3] = data[0];   
    data[sections*3+1] =  data[1];
    data[sections*3+2] = 0.0;
       
    
    /*
        transfer vertex data to the GPU
     */
    glGenBuffers(1, &m_vertexbuffer);               //generate a vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);  //bind buffer to be active
    glBufferData(GL_ARRAY_BUFFER, (3*sections+3)*sizeof(float), data, GL_STATIC_DRAW); //set buffer data
      
    glEnableClientState(GL_VERTEX_ARRAY);        //tell OpenGL that we want to specify vertex data in the buffer
    glVertexPointer(3, GL_FLOAT, 0, (void*)0);   //tell OpenGL where the vertex data lies in the buffer
    
    delete[] data;
}

/*
 Destroy Circle - clean up geometry
 */
Circle::~Circle()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);       //unbind current buffer
    glDeleteBuffers(1, &m_vertexbuffer);    //delete buffer data
}


/*
 Draw Circle
 */
void Circle::draw()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glDrawArrays(GL_LINE_STRIP, 0, sections+1);      //draw circle (tell OpenGL that buffer keeps line strip with [sections+1] lines)
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

