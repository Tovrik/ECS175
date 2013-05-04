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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "math.h"
#include "point.hpp"
#include "bitmap/bitmap.h"
#include "shader_utils.hpp"
//#include "shader_utils.cpp"


#include <iostream>

#define PI 3.1415926

using namespace std;

static const unsigned int m_width = 1000;
static const unsigned int m_height = 1000;
static const unsigned int numPoints = 32;
static GLfloat tM[16];

unsigned int m_texture;
unsigned int m_frame;
unsigned int m_program;               
unsigned int m_vertexsh;
unsigned int m_fragmentsh;  

Point data[numPoints];

GLfloat currX, currY, prevX, prevY;

void setPoints(){
	data[0].x = 10, data[0].y = 35,				//Back of tail
    data[1].x = 10, data[1].y = 45,
    data[2].x = 10, data[2].y = 65,
    data[3].x = 10, data[3].y = 65,
    
    data[4].x = 10, data[4].y = 65,				//Top of tail
    data[5].x = 10, data[5].y = 75,
    data[6].x = 25, data[6].y = 60,		
    data[7].x = 30, data[7].y = 55,			
    
    data[8].x = 30, data[8].y = 55,				//Top joint
    data[9].x = 33, data[9].y = 52,
    data[10].x = 36, data[10].y = 52,
    data[11].x = 40, data[11].y = 55,

    data[12].x = 40, data[12].y = 55,			//Top of body
    data[13].x = 70, data[13].y = 70,
    data[14].x = 75, data[14].y = 70,
    data[15].x = 85, data[15].y = 55,
   
    data[16].x = 85, data[16].y = 55,			//Nose
    data[17].x = 87, data[17].y = 52,	
    data[18].x = 87, data[18].y = 48,
    data[19].x = 85, data[19].y = 45,

    data[20].x = 85, data[20].y = 45,			//Bottom of body
    data[21].x = 75, data[21].y = 30,
    data[22].x = 70, data[22].y = 30,
    data[23].x = 40, data[23].y = 45,
    
    data[24].x = 40, data[24].y = 45,			//Bottom joint
    data[25].x = 36, data[25].y = 47,
    data[26].x = 33, data[26].y = 47,
    data[27].x = 30, data[27].y = 45,

    data[28].x = 30, data[28].y = 45,	      	//Bottom of tail
    data[29].x = 25, data[29].y = 40,
    data[30].x = 10, data[30].y = 25,
    data[31].x = 10, data[31].y = 35;       
}

/*
    drawDot: draws a dot
 */
void drawDot(int x, int y){
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
}

/*
    drawLine: draws a line
 */
void drawLine(Point p1, Point p2){
    glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();
}

/*
    findBezierPoint: finds the next point on the Bezier curve
 */
Point findBezierPoint(Point p1, Point p2, Point p3, Point p4, float t){
    Point temp(0,0);
    temp.x = (pow((1-t), 3) * p1.x) + (3 * t * pow((1-t), 2) * p2.x) + (3 * (1-t) * pow(t, 2) * p3.x) + (pow(t, 3) * p4.x);
    temp.y = (pow((1-t), 3) * p1.y) + (3 * t * pow((1-t), 2) * p2.y) + (3 * (1-t) * pow(t, 2) * p3.y) + (pow(t, 3) * p4.y);
    return temp;
}

/*
    drawBezierLine: uses results from findBezierPoint to draw the Bezier curve
 */
void drawBezierLine(int one, int two, int three, int four, Point temporaryPoint){
	for(float j = 0; j <= 1; j = j + 0.01) {
       	Point bezierPoint = findBezierPoint(data[one], data[two], data[three], data[four], j);
       	drawLine(temporaryPoint, bezierPoint);
       	temporaryPoint = bezierPoint;
    }
}

// void makeShaders()
// {
//      m_program = glCreateProgram();
    
//     char *shadercode = readShader("shader.vert");   //reads shader code (you can edit shader code with a text editor)
//     m_vertexsh = glCreateShader(GL_VERTEX_SHADER_ARB);
//     glShaderSource(m_vertexsh, 1, (const GLcharARB **)&shadercode,NULL);
//     delete[] shadercode;
//     glCompileShader(m_vertexsh);    //compiles shader
//     printInfoLog(m_vertexsh);       //prints errors during shader compilation
    
//     shadercode = readShader("shader.frag");     //reads shader code (you can edit shader code with a text editor)
//     m_fragmentsh = glCreateShader(GL_FRAGMENT_SHADER_ARB);
//     glShaderSource(m_fragmentsh, 1, (const GLcharARB **)&shadercode,NULL);
//     delete[] shadercode;
//     glCompileShader(m_fragmentsh);  //compiles shader
//     printInfoLog(m_fragmentsh);     //prints errors during shader compilation
    
//     glAttachShader(m_program,m_vertexsh);
//     glAttachShader(m_program,m_fragmentsh);

//     glLinkProgram(m_program);   //compiles fragment and vertex shader into a shader program
//     printInfoLog(m_program);    //prints errors during program compilation
    
//     glUseProgram(m_program);

//     int texture_location = glGetUniformLocation(m_program, "color_texture");
//     glUniform1i(texture_location, 0);
//     glUseProgram(0);
// }

// GLfloat * myTranslatef(GLfloat * m, GLfloat x, GLfloat y, GLfloat z) {
//     // 1st col
//     m[0] = 1.0f;
//     m[1] = 0.0f;
//     m[2] = 0.0f;
//     m[3] = 0.0f;
//     // 2nd col
//     m[4] = 0.0f;
//     m[5] = 1.0f;
//     m[6] = 0.0f;
//     m[7] = 0.0f;
//     // 3rd col
//     m[8] = 0.0f;
//     m[9] = 0.0f;
//     m[10] = 1.0f;
//     m[11] = 0.0f;
//     // 4rd col
//     m[12] = x;
//     m[13] = y;
//     m[14] = z;
//     m[15] = 1.0f;

//     glMultMatrixf(m);
//     return m;
// }

// GLfloat * myRotatef(GLfloat * m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
//     GLfloat len = sqrt(x*x+y*y+z*z);
//     x /= len;
//     y /= len;
//     z /= len;

//     GLfloat rangle = angle*PI/180.0f;
//     GLfloat vcos = cos(rangle);
//     GLfloat ivcos = 1.0f-vcos;
//     GLfloat vsin = sin(rangle);

//     // 1st col
//     m[0] = x*x*ivcos +vcos;
//     m[1] = x*y*ivcos +z*vsin;
//     m[2] = x*z*ivcos -y*vsin;
//     m[3] = 0.0f;
//     // 2nd col
//     m[4] = y*x*ivcos -z*vsin;
//     m[5] = y*y*ivcos +vcos;
//     m[6] = y*z*ivcos +x*vsin;
//     m[7] = 0.0f;
//     // 3rd col
//     m[8] = z*x*ivcos +y*vsin;
//     m[9] = z*y*ivcos -x*vsin;
//     m[10] = z*z*ivcos +vcos;
//     m[11] = 0.0f;
//     // 4th col
//     m[12] = 0.0f;
//     m[13] = 0.0f;
//     m[14] = 0.0f;
//     m[15] = 1.0f;

//     glMultMatrixf(m);
//     return m;
// }

/*
    draw: draws the fish to the screen
 */
void draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    Point tempPoint = data[0];
    setPoints();
    glColor3f(1,0,0);
    // for(int i = 0; i < numPoints; i++){ 		//draw vertecies and lines
    //     glColor3f(0,0,1);
    //     drawDot(data[i].x, data[i].y);
    //     glColor3f(1,0,0);
    //     if(i < numPoints - 1)
    //     	drawLine(data[i], data[i + 1]);
    // }
    glColor3f(1,1,1);

    drawBezierLine(0, 1, 2, 3, tempPoint);		//draw bezier curves
    tempPoint = data[3];
    drawBezierLine(4, 5, 6, 7, tempPoint);
    tempPoint = data[7];    
    drawBezierLine(8, 9, 10, 11, tempPoint);
    tempPoint = data[11];
    drawBezierLine(12, 13, 14, 15, tempPoint);
    tempPoint = data[15];
    drawBezierLine(16, 17, 18, 19, tempPoint);
    tempPoint = data[19];
    drawBezierLine(20, 21, 22, 23, tempPoint);  
    tempPoint = data[23];  
    drawBezierLine(24, 25, 26, 27, tempPoint);  
    tempPoint = data[27];  
    drawBezierLine(28, 29, 30, 31, tempPoint);  
    tempPoint = data[31];  

    //myTranslatef();

    glFlush();
}

void drawTexture(Point currPoint, Point prevPoint){
    glColor3f(1,1,1); //this is the bg color
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture); 
}


/*
 Initializes opengl states
 */
void initOpenGL()
{
    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        std::cout << "ERROR: Loading GLEW failed." << std::endl;
        exit(-1);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);       //default "empty"/background color is set to white
    
    //texture related code...

    // makeShaders();
    CBitmap image("fish.bmp");               //read bitmap image
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
    Main function: Program entry point
 */
int main (int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(m_width,m_height);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Assignment 2 - Bezier Curves");
	glutDisplayFunc(draw);
    //drawTexture();
	glClearColor(0,0,0,0);
    initOpenGL();
	glColor3f(1,0,0);
	glPointSize(8);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,100,0,100);
	glutMainLoop();
    
    return 0;
}