/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 1
 
*/

#ifdef __APPLE__
#include <OpenGL/gl.h> 
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h> 
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "glutwidget.hpp"
#include <iostream>


/*
 Initializes GLUT context
 */
glutWidget::glutWidget(int argc, char** argv)
{
    
    glutInitWindowSize(glutWidget::m_width, glutWidget::m_height);
    glutInit(&argc,argv);
    glutInitDisplayString("samples rgb double depth");
    glutCreateWindow("ECS 175 Demo 1");
    glutKeyboardUpFunc(keyUp);  //what to call when user releases a key
    glutDisplayFunc(render);    //what to call when window needs redrawing
    glutIdleFunc(update);       //what to call when no user input given
    initOpenGL();
}




/*
 Initializes opengl states
 */
void glutWidget::initOpenGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   //default "empty"/background color is set to white
}


/*
 Draws a rectangle with random color noise
 */
void drawNoise()
{
    const int dx = 100;
    const int dy = 100;
    unsigned char image[dx*dy*3];   //an array of dx*dy RGB values (3x8=24 bit of color per pixel)
    for(unsigned i = 0;i<dx*dy*3;++i)
        image[i] = rand()%255;      //random color initialization
     
    //draw the "image"
    glRasterPos2i(200,200);
    glDrawPixels(dx,dy,GL_RGB,GL_UNSIGNED_BYTE,image);   
}


/*
    Redraws window contents
 */
void glutWidget::render()
{
		
    glClear(GL_COLOR_BUFFER_BIT);           //clears color values of framebuffer
    
    glMatrixMode(GL_PROJECTION);       
    glLoadIdentity();                       //initializes projection matrix with identity
    glOrtho(0, m_width, 0, m_height, 0, 1);     //orthogonal projection is set up to match window size: 1 unit = 1 pixel
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                       //initializes modelview matrix with identity
    
    drawNoise();
   
    glutSwapBuffers();  //swaps front and back buffer for double buffering
}




/*
 Handles user event: a key was released
 */
void glutWidget::keyUp(unsigned char key, int, int) 
{  
    if(key == 'q'       //"q"
    || (int)key == 27) //escape key
    { 
        stop(); //quit the main loop, exit program
    } 
}




/*
    Forces program to stop, cleans up, and exits
 */
void glutWidget::stop()
{
    exit(0);
}




/*
    Starts the main loop
 */
void glutWidget::run()
{
    glutMainLoop();
}




/*
    Called whenever no user input given
 */
void glutWidget::update()
{
    glutPostRedisplay(); //marks window for redrawing
}



    
