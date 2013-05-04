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

#include "glutwidget.hpp"
#include "shader_utils.hpp"
#include "cube.hpp"
#include <iostream>

int glutWidget::m_pos_attribute_location;    
int glutWidget::m_normal_attribute_location;
unsigned int glutWidget::m_frame;
unsigned int glutWidget::m_program;               
unsigned int glutWidget::m_vertexsh;
unsigned int glutWidget::m_fragmentsh;  
Cube *glutWidget::c;


/*
 Initializes GLUT context
 */
glutWidget::glutWidget(int argc, char** argv)
{
    m_frame = 0;
    glutInitWindowSize(glutWidget::m_width, glutWidget::m_height);
    glutInit(&argc,argv);
    glutInitDisplayString("samples rgb double depth");
    glutCreateWindow("ECS 175 Demo 4");
    glutKeyboardUpFunc(keyUp);  //what to call when user releases a key
    glutDisplayFunc(render);    //what to call when window needs redrawing
    glutIdleFunc(update);       //what to call when no user input given
    initOpenGL();
}


/*
 Checks whether graphics driver supports all required OpenGL features
 */
void glutWidget::checkExtensions()
{
    //query some extensions to make sure they are present
    if(glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
    {
        std::cout << "ERROR: Shading language extension not present." << std::endl;
    }
    if(glewGetExtension("GL_ARB_vertex_program") != GL_TRUE)
    {
        std::cout << "ERROR: Vertex program extension not present." << std::endl;
    }
    if(glewGetExtension("GL_ARB_vertex_shader") != GL_TRUE)
    {
        std::cout << "ERROR: Vertex shader extension not present." << std::endl;
    }
    if(glewGetExtension("GL_ARB_fragment_program") != GL_TRUE)
    {
        std::cout << "ERROR: Fragment program extension not present." << std::endl;
    }
    if(glewGetExtension("GL_ARB_fragment_shader") != GL_TRUE)
    {
        std::cout << "ERROR: Fragment shader extension not present." << std::endl;
    }
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
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    
    makeShaders();          //load data of fragment and vertex programs/shaders - compile shaders
    
    c = &Cube::init(m_pos_attribute_location,m_normal_attribute_location);      //create cube vertex buffer object (VBO)
}



/*
 Draws four cubes.
 Draws cubes using VBOs (replaces old glBegin/glEnd calls)
 */
void glutWidget::drawCubes()
{
    glUseProgram(m_program);        //enable use of vertex and fragment shader (we are now using programmable parts of pipeline)
                                    //try opening the shader code in the "shaders" subfolder with a texteditor to see what they do
                                    //the vertex shader will operate an all vertices that are passed to the GPU in the following
                                    //the fragment shader wil operate on all fragments covered by the following geometry
    
    glRotated(m_frame/100,1,1,1);   //add a rotation to MODELVIEW MATRIX
    
    glPushMatrix();
    glTranslated(0.5,0.5,0.5);      
    c->drawCube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,-1.5,-1.5);
    c->drawCube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,0.5,-1.5);
    c->drawCube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,0.5,0.5);
    c->drawCube();
    glPopMatrix();

    
    glPushMatrix();
    glTranslated(0.5,-1.5,0.5);
    c->drawCube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0.5,-1.5,-1.5);
    c->drawCube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0.5,0.5,-1.5);
    c->drawCube();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-1.5,-1.5,0.5);
    c->drawCube();
    glPopMatrix();
    
    glUseProgram(0);    //disable use of shader
    
       
}


/*
    Redraws window contents
 */
void glutWidget::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     //clears color and depth bits of framebuffer
    
    glMatrixMode(GL_PROJECTION);       
    glLoadIdentity();                                             //initializes projection matrix with identity
    gluPerspective(60,(float)m_width/(float)m_height,0.1,100);  //set up projection mode (field of view, aspect ratio, near and far clipping plane)
    
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
    //clean up shaders
    glDetachShader(m_program,m_vertexsh);
	glDetachShader(m_program,m_fragmentsh);
    
    glDeleteProgram(m_program);
	glDeleteShader(m_vertexsh);
	glDeleteShader(m_fragmentsh);
    
    exit(0);
}

/*
    Reads in and compiles shader objects
 */
void glutWidget::makeShaders()
{
    m_program = glCreateProgram();
	
	char *shadercode = readShader("shaders/vertexshader.vert");	//reads shader code (you can edit shader code with a text editor)
    m_vertexsh = glCreateShader(GL_VERTEX_SHADER_ARB);
    glShaderSource(m_vertexsh, 1, (const GLcharARB **)&shadercode,NULL);
    delete[] shadercode;
    glCompileShader(m_vertexsh);    //compiles shader
    printInfoLog(m_vertexsh);       //prints errors during shader compilation
    
    
	shadercode = readShader("shaders/fragmentshader.frag");     //reads shader code (you can edit shader code with a text editor)
    m_fragmentsh = glCreateShader(GL_FRAGMENT_SHADER_ARB);
	glShaderSource(m_fragmentsh, 1, (const GLcharARB **)&shadercode,NULL);
	delete[] shadercode;
	glCompileShader(m_fragmentsh);  //compiles shader
	printInfoLog(m_fragmentsh);     //prints errors during shader compilation
    
	glAttachShader(m_program,m_vertexsh);
	glAttachShader(m_program,m_fragmentsh);

	glLinkProgram(m_program);   //compiles fragment and vertex shader into a shader program
	printInfoLog(m_program);    //prints errors during program compilation
    
    glUseProgram(m_program);
    m_pos_attribute_location = glGetAttribLocation(m_program,"in_Position");  //get pointer to "in_Position" attribute of vertex shader
    m_normal_attribute_location = glGetAttribLocation(m_program,"in_Normal"); //get pointer to "in_Normal" attribute of vertex shader
    glUseProgram(0);
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



    
