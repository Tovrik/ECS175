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

#include "glutwidget.hpp"
#include <iostream>

Animal *glutWidget::c;

/*
 Initializes GLUT context
 */
glutWidget::glutWidget(int argc, char** argv)
{
    glutInitWindowSize(glutWidget::m_width, glutWidget::m_height);
    glutInit(&argc,argv);
    //glutInitDisplayString("samples rgb double depth");
    glutCreateWindow("ECS 175 Assignment 2");
    // glutKeyboardUpFunc(keyUp);  //what to call when user releases a key
    glutDisplayFunc(render);    //what to call when window needs redrawing
    // glutIdleFunc(update);       //what to call when no user input given
    initOpenGL();
    
    c = new Animal(); //create new Animal

}

/*
 Checks whether graphics driver supports all required OpenGL features
 */
void glutWidget::checkExtensions()
{
    //query some extensions to make sure they are present
    if(glewGetExtension("GL_ARB_vertex_buffer_object") != GL_TRUE)
    {
        std::cout << "ERROR: VBO extension not present." << std::endl;
    }    
}


/*
 Initializes opengl states
 */
void glutWidget::initOpenGL()
{
    glewExperimental = GL_TRUE; 
    GLenum err = glewInit();                             //initialize GLEW - this enables us to use extensions
    if(err != GLEW_OK)
    {
        std::cout << "ERROR: Loading GLEW failed." << std::endl;
        exit(-1);
    }
    checkExtensions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   //default "empty"/background color is set to white
}

/*
    Redraws window contents
 */
void glutWidget::render()
{
		
    // glClear(GL_COLOR_BUFFER_BIT);           //clears color values of framebuffer
    
    // glMatrixMode(GL_PROJECTION);       
    // glLoadIdentity();                       //initializes projection matrix with identity
    // glOrtho(0, m_width, 0, m_height, 0, 1);     //orthogonal projection is set up to match window size: 1 unit = 1 pixel
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();                       //initializes modelview matrix with identity
    
    // glColor3f(0,0,0);   //set drawing color to black
    c->draw();          //draw animal
   
    // glutSwapBuffers();  //swaps front and back buffer for double buffering
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
    delete c;   //delete circle
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
