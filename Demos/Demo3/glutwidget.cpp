/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 3
 
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

unsigned int glutWidget::m_frame;

/*
 Initializes GLUT context
 */
glutWidget::glutWidget(int argc, char** argv)
{
    m_frame = 0;
    glutInitWindowSize(glutWidget::m_width, glutWidget::m_height);
    glutInit(&argc,argv);
    glutInitDisplayString("samples rgb double depth");
    glutCreateWindow("ECS 175 Demo 3");
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
    
    //initialize lighting
	glEnable(GL_LIGHTING);  //enable lighting
    
	glEnable(GL_LIGHT0);
	GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);      //set ambient color
	GLfloat diffuse[] = {0.9, 0.9, 0.9, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);      //set diffuse color
    GLfloat position[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, position);    //set position
    
    glEnable(GL_NORMALIZE);     //enable normalization (normals are normalized automatically after scaling)
    glEnable(GL_CULL_FACE);     //enable culling (discards backfaces to improve performance)
    glEnable(GL_DEPTH_TEST);   //enable depth testing (z-buffering)
}


/*
 Draw a single cube
 */
void cube()
{
    //using obsolete glBegin/glEnd calls here (see Demo4 for vertex buffer objects (VBO) use according to newer OpenGL specs)
    glBegin(GL_QUADS);
    //bottom
    glNormal3f(0,-1,0); //normal of the following vertices (bottom face)
    glVertex3f(0,0,0);  //four vertices of bottom face
    glVertex3f(1,0,0);
    glVertex3f(1,0,1);
    glVertex3f(0,0,1);
	
    // Top 
    glNormal3f(0,1,0);  //normal of the following vertices (top face)
    glVertex3f(0,1,0);  //four vertices of top face
    glVertex3f(0,1,1);
    glVertex3f(1,1,1);
    glVertex3f(1,1,0);
	
    // Front 
    glNormal3f(0,0,1);  //normal of the following vertices (front face)
    glVertex3f(0,0,1);  //four vertices of front face
    glVertex3f(1,0,1);
    glVertex3f(1,1,1);
    glVertex3f(0,1,1);
	
    // Back 
    glNormal3f(0,0,-1);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glVertex3f(1,1,0);
    glVertex3f(1,0,0);
    
    // Right 
    glNormal3f(0,0,1);
    glVertex3f(1,0,0);
    glVertex3f(1,1,0);
    glVertex3f(1,1,1);
    glVertex3f(1,0,1);
	
    // Left 
    glNormal3f(0,0,-1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glVertex3f(0,1,1);
    glVertex3f(0,1,0);
    
    glEnd();
}


/*
 Draws four cubes
 */
void glutWidget::drawCubes()
{
    glRotated(m_frame/100,1,1,1); //rotate around axis (1,1,1) by angle [m_frame/100]
    
    glPushMatrix();
    glTranslated(0.5,0.5,0.5);  //this cube is translated by (0.5,0.5,0.5)
    cube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,-1.5,-1.5);   //this cube is translated by (-1.5,-1.5,-1.5)
    cube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,0.5,-1.5);
    cube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,0.5,0.5);
    cube();
    glPopMatrix();
    
    
    glPushMatrix();
    glTranslated(0.5,-1.5,0.5);
    cube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0.5,-1.5,-1.5);
    cube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0.5,0.5,-1.5);
    cube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,-1.5,0.5);
    cube();
    glPopMatrix();
    
       
}


/*
    Redraws window contents
 */
void glutWidget::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     //clears color and depth bits of framebuffer
    
    glMatrixMode(GL_PROJECTION);       
    glLoadIdentity();                                           //initializes projection matrix with identity
    gluPerspective(60,(float)m_width/(float)m_height,0.1,100);  //set up projection mode (field of view, aspect ratio, and near and far clipping planes)
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();                       //initializes modelview matrix with identity
    gluLookAt(0,0,10,0,0,0,0,1,0);          //set up camera position and orientation

    drawCubes();
   
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
    m_frame++;
    glutPostRedisplay(); //marks window for redrawing
}



    
