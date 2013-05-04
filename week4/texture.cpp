#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <algorithm>
#include "bitmap/bitmap.h"
#include "shader_utils.hpp"

#define KEY_ESCAPE 27
#define PI 3.1415926

static int screen_x, screen_y;
static char message[1024];
static char * ptr = message;
static int LOD = 5;

static float px, py, pz;
static float tx, ty, tz;

static GLuint texture1, texture2;

static GLfloat tM[16];
static GLfloat rM[16];

unsigned int m_frame;
unsigned int m_program;               
unsigned int m_vertexsh;
unsigned int m_fragmentsh;  

GLfloat cpoints[][3] = {
        {100.0, 100.0, 0.0}, {300.0, 400.0, 0.0},
        {500.0, 400.0, 0.0}, {600.0, 500.0, 0.0},
        {700.0, 100.0, 0.0}, {400.0, 100.0, 0.0}
};
size_t num_cpoints = 5;

struct glutWindow {
    int width;
    int height;
    char * title;

    float field_of_view_angle;
    float z_near;
    float z_far;
} w;

//glutWindow w;

void drawBitmapText(char * text, float x, float y, float z) 
{  
    char * c;
    glRasterPos3f(x, y, z);
    glColor3f(1.0f, 1.0f, 1.0f);
    for(c=text; c[0]!=0; ++c)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c[0]);
}

void makeShaders()
{
    m_program = glCreateProgram();
	
	char *shadercode = readShader("shader.vert");	//reads shader code (you can edit shader code with a text editor)
    m_vertexsh = glCreateShader(GL_VERTEX_SHADER_ARB);
    glShaderSource(m_vertexsh, 1, (const GLcharARB **)&shadercode,NULL);
    delete[] shadercode;
    glCompileShader(m_vertexsh);    //compiles shader
    printInfoLog(m_vertexsh);       //prints errors during shader compilation
    
	shadercode = readShader("shader.frag");     //reads shader code (you can edit shader code with a text editor)
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
//    m_pos_attribute_location = glGetAttribLocation(m_program,"in_Position");  //get pointer to "in_Position" attribute of vertex shader
//    m_normal_attribute_location = glGetAttribLocation(m_program,"in_Normal"); //get pointer to "in_Normal" attribute of vertex shader
    int texture_location = glGetUniformLocation(m_program, "color_texture");
    glUniform1i(texture_location, 0);
    glUseProgram(0);
}

void bezier(float t, float & x, float & y, float & z) {
    // nice to pre-compute 1-t because we will need it frequently
    float it = 1.0f -t;

    // calculate blending functions
    float b0 = 1*t*t*t*t;
    float b1 = 4*t*t*t*it;
    float b2 = 6*t*t*it*it;
    float b3 = 4*t*it*it*it;
    float b4 = 1*it*it*it*it;

    // calculate the x,y and z of the curve point by summing
    // the Control vertices weighted by their respective blending
    // functions
    x = b0*cpoints[0][0] +
        b1*cpoints[1][0] +
        b2*cpoints[2][0] +
        b3*cpoints[3][0] +
        b4*cpoints[4][0];

    y = b0*cpoints[0][1] +
        b1*cpoints[1][1] +
        b2*cpoints[2][1] +
        b3*cpoints[3][1] +
        b4*cpoints[4][1];

    z = b0*cpoints[0][2] +
        b1*cpoints[1][2] +
        b2*cpoints[2][2] +
        b3*cpoints[3][2] +
        b4*cpoints[4][2];
}

GLfloat * myTranslatef(GLfloat * m, GLfloat x, GLfloat y, GLfloat z) {
    // 1st col
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    // 2nd col
    m[4] = 0.0f;
    m[5] = 1.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    // 3rd col
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;
    // 4th col
    m[12] = x;
    m[13] = y;
    m[14] = z;
    m[15] = 1.0f;

    glMultMatrixf(m);
    return m;
}

GLfloat * myRotatef(GLfloat * m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat len = sqrt(x*x+y*y+z*z);
    x /= len;
    y /= len;
    z /= len;

    GLfloat rangle = angle*PI/180.0f;
    GLfloat vcos = cos(rangle);
    GLfloat ivcos = 1.0f-vcos;
    GLfloat vsin = sin(rangle);

    // 1st col
    m[0] = x*x*ivcos +vcos;
    m[1] = x*y*ivcos +z*vsin;
    m[2] = x*z*ivcos -y*vsin;
    m[3] = 0.0f;
    // 2nd col
    m[4] = y*x*ivcos -z*vsin;
    m[5] = y*y*ivcos +vcos;
    m[6] = y*z*ivcos +x*vsin;
    m[7] = 0.0f;
    // 3rd col
    m[8] = z*x*ivcos +y*vsin;
    m[9] = z*y*ivcos -x*vsin;
    m[10] = z*z*ivcos +vcos;
    m[11] = 0.0f;
    // 4rd col
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;

    glMultMatrixf(m);
    return m;
}

void display() {
    //clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT);

    float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f * 20.0f;
    float u = glutGet(GLUT_ELAPSED_TIME) / 10000.0f * 3;
    float q = fabs(cos(u));
    u = q;

    float mx, my, mz;
    bezier(u, mx, my, mz);

    tx = mx - px;
    ty = my - py;
    tz = mz - pz;
    float len = sqrt(tx*tx+ty*ty+tz*tz);
    float vtx = ty;
    float vty = -tx;
    float vtz = tz;
    vtx /= len;
    vty /= len;
    vtz /= len;

    px = mx;
    py = my;
    pz = mz;

    drawBitmapText(message, 0.0f, (float)w.height-75.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
//    glBegin(GL_LINE_STRIP);
        for(int i=0; i<=LOD; ++i) {
            float t = (float)i / (LOD);
            float tx, ty, tz;
            bezier(t, tx, ty, tz);
            glVertex3f(tx, ty, tz);
        }
//        for(int i=0; i<=LOD; i++)
//            glEvalCoord1f( (GLfloat)i /(GLfloat)LOD );
    glEnd();

    /* The following code displays the control polygon */
    glLineWidth(3.0f);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINE_STRIP);
        for(size_t i=1; i<num_cpoints; i++) {
            glVertex3fv(&cpoints[i-1][0]);
            glVertex3fv(&cpoints[i][0]);
        }
    glEnd();
    //
    glPointSize(10.0f);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
        for(size_t i=0; i<num_cpoints; i++)
            glVertex3fv(&cpoints[i][0]);
    glEnd();
    glFlush();

    glColor3f(1.0, 1.0, 1.0);

    // push current matrix onto stack
    glPushMatrix();

    // v' = (M1*M2*M3)*v
    //glTranslatef(250.0f,250.0f,0.0f);       //M1
    //glRotatef(t, 0.0f,0.0f,1.0f);           //M2
    //glTranslatef(-250.0f,-250.0f,0.0f);     //M3
    myTranslatef(tM, mx, my, mz);
    glTranslatef(50*vtx, 50*vty, 50*vtz);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, -50.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-50.0f,  50.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 50.0f,  50.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 50.0f, -50.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, -30.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f,  30.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 30.0f,  30.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 30.0f, -30.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // pop current matrix out of stack
    myRotatef(rM, 10*t, 0.0f,0.0f,1.0f);         //M2


    glLineWidth(5.0f);
    glBegin(GL_LINES);
        glColor3f(1.0f,0.0f,0.0f); glVertex3f(-30.0f,-30.0f,0.0f);
        glColor3f(0.0f,1.0f,0.0f); glVertex3f( 30.0f, 30.0f,0.0f);
        glColor3f(0.0f,1.0f,0.0f); glVertex3f(-30.0f, 30.0f,0.0f);
        glColor3f(0.0f,0.0f,1.0f); glVertex3f( 30.0f,-30.0f,0.0f);
    glEnd();

    glPointSize(10.0);
    glBegin(GL_POINTS);
        glColor3f(0.0f,0.0f,1.0f); glVertex3f(0.0f,0.0f,0.0f);
        glColor3f(0.0f,1.0f,0.0f); glVertex3f(-30.0f,0.0f,0.0f);
    glEnd();
    glPointSize(6.0);

    glPopMatrix();

    //double buffering
    glutSwapBuffers();
}


void initialize() {

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();    //initialize GLEW - this enables us to use extensions
    if(err != GLEW_OK) {
        std::cout << "ERROR: Loading GLEW failed." << std::endl;
        exit(-1);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (float)w.width, 0.0f, (float)w.height, 0.0f, 1.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //XXX
    makeShaders();

    //XXX
    CBitmap image("txt.bmp");                  //read bitmap image
    glGenTextures(1, &texture1);               //allocate space for texture
    glBindTexture(GL_TEXTURE_2D, texture1);    //bind the texture to be active
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetBits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);   //specify minificaton filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //specify magnificaton filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       //specify texture coordinate treatment
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       //specify texture coordinate treatment

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, num_cpoints, &cpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);

    //
    tx = cpoints[1][0] - cpoints[0][0];
    ty = cpoints[1][1] - cpoints[0][1];
    tz = cpoints[1][2] - cpoints[0][2];
    px = cpoints[0][0] - 1e-2*tx;
    py = cpoints[0][1] - 1e-2*ty;
    pz = cpoints[0][2] - 1e-2*tz;
}


void reshape(int width, int height) {
    w.width = width;
    w.height = height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (float)w.width, 0.0f, (float)w.height, 0.0f, 1.0f);
}

void keyboard(unsigned char key, int mousePositionX, int mousePositionY) {

    //snprintf(message, 1024, "%c", key);
    if(ptr-message >= 16) {
        memset(message, 0,sizeof(message));
        ptr = message;
    }
    *ptr = key;
    ptr++;

    switch(key) {
        case KEY_ESCAPE:
            exit(0);
            break;
        case '1':
            screen_x = mousePositionX;
            screen_y = mousePositionY;
            printf("x %d y %d\n", mousePositionX, mousePositionY);
            break;
        case '+':
            LOD++;
            break;
        case '-':
            LOD = std::max(5, --LOD);
            break;
        default:
            break;
    }
}

void mouseButton(int button, int state, int x, int y) {
    y = w.height -y;

    static int selectIdx = -1;
    static int startx=0, starty=0;

    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            printf("Down at (%d,%d)\n", x,y);
            for(int i=0; i<6; ++i) {
                if( (fabs(x-(int)cpoints[i][0]) <50) && (fabs(y-(int)cpoints[i][1]) <50) ) {
                    selectIdx = i;
                    startx = x;
                    starty = y;
                    printf("click on %d\n", selectIdx);
                    break;
                }
            }
        }
        else {
            if(selectIdx ==-1) return;

            printf("Up at (%d,%d)\n", x,y);
            printf("Move (%d,%d)\n", x-startx,y-starty);
            int endx=x, endy=y;
            cpoints[selectIdx][0] += (endx-startx);
            cpoints[selectIdx][1] += (endy-starty);
            selectIdx = -1;
            startx=0; starty=0;

            glDisable(GL_MAP1_VERTEX_3);
            glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, num_cpoints, &cpoints[0][0]);
            glEnable(GL_MAP1_VERTEX_3);
        }
    }
    fflush(stdout);
}

void mouseMotion(int x, int y) {
    printf("x %d y %d\n", x,y);
    fflush(stdout);
}

void deleteTextures() {
    if(texture1)
        glDeleteTextures(1, &texture1);
    if(texture2)
        glDeleteTextures(1, &texture2);
}


int main(int argc, char * argv[]) {

    // set window values
    w.width = 1024;
    w.height = 768;
    w.title = "Hello Triangle!";
    w.field_of_view_angle = 45;
    w.z_near = 1.0f;
    w.z_far = 50.0f;

    // initialize and run program
    glutInit(&argc, argv);                                      // GLUT initialization
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);   // `Display Mode'
    glutInitWindowSize(w.width, w.height);                      // set window size
    glutInitWindowPosition(10,10);
    glutCreateWindow(w.title);                                  // create Window

    glutDisplayFunc(display);                                   // register `Display Function'
    glutIdleFunc(display);                                      // register `Idle Function'
    glutReshapeFunc(reshape);                                   // register `Reshape Function'
    glutKeyboardFunc(keyboard);                                 // register `Keyboard Handler'
    glutMouseFunc(mouseButton);                                 // register `Mouse Button Handler'
    glutMotionFunc(mouseMotion);                                // register `Mouse Motion Handler'

    initialize();
    glutMainLoop();                                             // run GLUT mainloop
    deleteTextures();

    return EXIT_SUCCESS;
}

