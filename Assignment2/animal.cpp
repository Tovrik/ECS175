/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Stefan Peterson
 SID#: 998556530 
 Assignment 2
 
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
#include "animal.hpp"
#include <iostream>


Point* data = new Point[24];

 Animal::Animal(){


    // this->sections = sections;
    // const int numSections = 24;
    /*
        sampling the parametric form of the curve (circle) to create lines for rendering
    */
    
    //vertex data of circle (3 floats per position= 3*sections floats)
    // float *data = new float[3*sections+3];      //we duplicate the first point to get a closed circle - this needs 3 additional floats

    // float u = 0.0;                              //current evaluation parameter
    // const float du = (2*M_PI)/(float)sections;  //change of curve parameter between two samples
    // for(int iu = 0;iu<sections;++iu)            //create [sections] samples of the parametric circle
    // {
    //     data[iu*3] = mx + radius*sin(u);        //x coordinate for current u parameter value
    //     data[iu*3+1] =  my + radius*cos(u);     //y coordinate for current u parameter value
    //     data[iu*3+2] = 0;                       //z coordinate = 0
    //     u += du;                                //go to parameter value of next sample point
    // }
    
    //Hardcode my points
 //    data[0] = mx/10; 						//first point
 //    data[1] = my/10;
 //    data[2] = 0;

 //    data[3] = mx/10;						//second point
 //    data[4] = my - my/10;
 //    data[5] = 0;

 //    data[6] = mx/5;							//third point
 //    data[7] = my/2 + my/10;
 //    data[8] = 0;
    
 //    data[9] = mx - mx/10; 					//fourth point
 //    data[10] = my/2;
 //    data[11] = 0;

 //    data[12] = mx/5;							//fifth point
 //    data[13] = my/2 - my/10;
	// data[14] = 0;

 //    data[numSections - 3] = data[0];		//last point (same as first point)
 //    data[numSections - 2] = data[1];
 //    data[numSections - 1] = data[2];

    //make first and last point identical to get closed circle
    // data[sections*3] = data[0];   
    // data[sections*3+1] =  data[1];
    // data[sections*3+2] = 0.0;
       
    
    /*
        transfer vertex data to the GPU
     */
    // glGenBuffers(1, &m_vertexbuffer);               //generate a vertex buffer object
    // glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);  //bind buffer to be active
    // glBufferData(GL_ARRAY_BUFFER, (numSections)*sizeof(float), data, GL_STATIC_DRAW); //set buffer data
      
    // glEnableClientState(GL_VERTEX_ARRAY);        //tell OpenGL that we want to specify vertex data in the buffer
    // glVertexPointer(3, GL_FLOAT, 0, (void*)0);   //tell OpenGL where the vertex data lies in the buffer
    
    // delete[] data;
}
/*
 Destroy Animal - clean up geometry
 */
 Animal::~Animal(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);       //unbind current buffer
    glDeleteBuffers(1, &m_vertexbuffer);    //delete buffer data
 }

/*
 Draw Animal
 */
void Animal::drawDot(int x, int y){
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
    // glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    // glEnableClientState(GL_VERTEX_ARRAY);
    
    // glDrawArrays(GL_LINE_STRIP, 0, sections+1);      //draw circle (tell OpenGL that buffer keeps line strip with [sections+1] lines)
    
    // glDisableClientState(GL_VERTEX_ARRAY);
    // glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void Animal::drawLine(Point p1, Point p2){
    glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();
}

Point Animal::findBezierPoint(Point p1, Point p2, Point p3, Point p4, float t){
    Point temp;
    temp.x = (pow((1-t), 3) * p1.x) + (3 * t * pow((1-t), 2) * p2.x) + (3 * (1-t) * pow(t, 2) * p3.x) + (pow(t, 3) * p4.x);
    temp.y = (pow((1-t), 3) * p1.y) + (3 * t * pow((1-t), 2) * p2.y) + (3 * (1-t) * pow(t, 2) * p3.y) + (pow(t, 3) * p4.y);
    return temp;
}

void Animal::draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    Point tempPoint = data[0];
    data[0].x = 0, data[0].y = 0,
    data[1].x = 2, data[1].y = 5,
    data[2].x = 5, data[2].y = 9,
    data[3].x = 10, data[3].y = 20;
    glColor3f(1,0,0);
    for(int i = 0; i < 4; i++){
        drawDot(data[1].x, data[i].y);
    }
    glColor3f(1,1,1);
    drawLine(data[0], data[1]);
    drawLine(data[1], data[2]);
    drawLine(data[2], data[3]);
    for(float j = 0; j <= 1; j = j + 0.1){
        Point bezierPoint = findBezierPoint(data[0], data[1], data[2], data[3], j);
        drawLine(tempPoint, bezierPoint);
        tempPoint = bezierPoint;
    }
    glFlush();
    delete[] data;
}

