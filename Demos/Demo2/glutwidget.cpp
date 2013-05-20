/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 2
 
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
#include <string.h>
#include "bitmap/bitmap.h" //Bitmap loader by Benjamin Kalytta http://www.kalytta.com/bitmap.h

unsigned int glutWidget::m_texture;
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
    glutCreateWindow("ECS 175 Demo 2");
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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);       //default "empty"/background color is set to white
    
    CBitmap image("texture.bmp");               //read bitmap image
    
    glGenTextures(1,&m_texture);                //allocate 1 texture
    glBindTexture(GL_TEXTURE_2D, m_texture);    //bind this texture to be active
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.GetWidth(),image.GetHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,image.GetBits()); //load data into texture
        
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //specify minificaton filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //specify magnificaton filtering

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);    //specify texture coordinate treatment
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);    //specify texture coordinate treatment
    glEnable(GL_TEXTURE_2D);    //enable texturing
}


/*
 Draw a single textured quad
 */
void quad()
{
    //using obsolete gBegin/glEnd calls (see Demo4 for vertex buffer objects (VBO))
    
    glBegin(GL_QUADS);      //tell OpenGL that the following vertices form a QUAD
        glTexCoord2i(0,0); //texture coordinate of first vertex
        glVertex2i(0,0);   //first vertex
        
        glTexCoord2i(1,0); //texture coordinate of second vertex
        glVertex2i(1,0);   //second vertex
        
        glTexCoord2i(1,1);  //texture coordinate of third vertex
        glVertex2i(1,1);    //third vertex

        glTexCoord2i(0,1); //texture coordinate of fourth vertex
        glVertex2i(0,1);    //fourth vertex
    glEnd();
}


/*
 Draws a rectangle with texture
 */
void glutWidget::drawCircles()
{
    glEnable(GL_BLEND);                                 //enable alpha blending
    glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_ALPHA);   //specifies blending function for textures with alpha
    
    glTranslated((m_frame/100)%m_width,0,0);    //move the following by [(m_frame/100)%m_width] units in x direction
    
    glColor3f(1.0f,0.0f,0.0f);  //change color to red
    glPushMatrix();             //push current modelview matrix onto stack
    glScaled(100,100,1);        //scale by 100 units in x and y
    quad();
    glPopMatrix();              //restore modelview matrix before scaling
    
    glColor3f(0.0f,1.0f,0.0f);  //change color to green
    glTranslated(0,200,0);      //translate by 200 units in y direction
    glPushMatrix();             //push current modelview matrix onto stack
    glScaled(100,100,1);        //scale by 100 units in x and y
    quad();
    glPopMatrix();              //restore modelview matrix before scaling
    
    glColor3f(0.0f,0.0f,1.0f);  //set color to blue
    glTranslated(0,200,0);      //translate by 200 units in y direction
    glPushMatrix();             //push current modelview matrix onto stack
    glScaled(100,100,1);        //scale by 100 units in x and y
    quad();
    glPopMatrix();              //restore modelview matrix before scaling

    glDisable(GL_BLEND);                                //disable alpha blending
}


/*
    Redraws window contents
 */
void glutWidget::render()
{
    glClear(GL_COLOR_BUFFER_BIT);               //clears color values of framebuffer
    
    glMatrixMode(GL_PROJECTION);                //current matrix is PROJECTION
    glLoadIdentity();                           //initializes projection matrix with identity
    glOrtho(0, m_width, 0, m_height, 0, 1);     //orthogonal projection is set up to match window size: 1 unit = 1 pixel
    glMatrixMode(GL_MODELVIEW);                 //current matrix is MODELVIEW
    glLoadIdentity();                           //initializes modelview matrix with identity
    
    drawCircles();
   
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
    glDeleteTextures(1,&m_texture); //delete texture
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



    
