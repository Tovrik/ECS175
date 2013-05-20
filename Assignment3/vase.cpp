#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "bitmap/bitmap.h"

#define PI 3.1415926
#define KEY_ESCAPE 27
using namespace std;

unsigned int m_frame;
unsigned int m_program;               
unsigned int m_vertexsh;
unsigned int m_fragmentsh;  

static int screen_x, screen_y;
static char message[1024];
static char * ptr = message;
static int degree = 20;
static float t;
GLdouble eyeX = 0.0f, eyeY = 6.0f, eyeZ = 10.0f, 
centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f, 
upX = 0.0f, upY = 1.0f, upZ = 0.0f;
double mouseX, mouseY;
double cameraRadius = 4.5;
double cameraCenter[3] = {0.0, 0.0, 0.0};
double cameraRotate[2] = {0.0, 0.0};
double cameraPosition[3] = {
    camera_center[0] + camera_radius*cos(radians(camera_rot[0]))*cos(radians(camera_rot[1])),
    camera_center[1] + camera_radius*sin(radians(camera_rot[1])),
    camera_center[2] + camera_radius*sin(radians(camera_rot[0]))*cos(radians(camera_rot[1]))
}

static unsigned int m_texture;

GLfloat M[16];

GLfloat cpoints[][3] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 0.5f, 0.0f},
        {2.5f, 1.0f, 0.0f},
        {0.5f, 2.0f, 0.0f},
        {0.5f, 2.5f, 0.0f}, 
        {1.0f, 3.0f, 0.0f}
};
size_t npts = 20;
size_t sec = 2000;

struct glutWindow {
    int width;
    int height;
    char * title;

    float field_of_view_angle;
    float z_near;
    float z_far;
} w;

//glutWindow w;

struct vec4f {
    float x, y, z, w;
    vec4f() : x(0.0f),y(0.0f),z(0.0f),w(1.0f) {
    }
    vec4f(float x, float y, float z) : x(x),y(y),z(z),w(1.0f) {
    }
    vec4f(float x, float y, float z, float w) : x(x),y(y),z(z),w(w) {
    }
    //getter
    float operator[] (size_t i) const {
        switch(i) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
    }
    //setter
    float & operator[] (size_t i) {
        switch(i) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
    }
    vec4f operator- (const vec4f & rhs) const {
        vec4f res(*this);
        res.x -= rhs.x;
        res.y -= rhs.y;
        res.z -= rhs.z;
        return res;
    }
    float norm() const {
        float len = 0.0;
        len = sqrt(x*x+y*y+z*z);
        return len;
    }
    void normalize() {
        float len = norm();
        x /= len;
        y /= len;
        z /= len;
    }
    vec4f cross(const vec4f & rhs) const {
        vec4f out;
        out.x = y*rhs.z - z*rhs.y;
        out.y = z*rhs.x - x*rhs.z;
        out.z = x*rhs.y - y*rhs.x;
        return out;
    }
};

vec4f multiply(GLfloat * m, vec4f & v) {
    // v' = m*v
    vec4f v_new;
    for(size_t i=0; i<4; ++i) {
        GLfloat vi = v[i];
        for(size_t j=0; j<4; ++j) {
            v_new[j] += m[4*i+j]*vi;
        }
    }
    return v_new;
}


void bezier(float t, float & x, float & y, float & z) {
    // nice to pre-compute 1-t because we will need it frequently
    float it = 1.0f -t;

    // calculate blending functions
    float b0 = 1*t*t*t*t*t;
    float b1 = 5*t*t*t*t*it;
    float b2 = 10*t*t*t*it*it;
    float b3 = 10*t*t*it*it*it;
    float b4 = 5*t*it*it*it*it;
    float b5 = 1*it*it*it*it*it;
    // calculate the x,y and z of the curve point by summing
    // the Control vertices weighted by their respective blending
    // functions
    x = b0*cpoints[0][0] +
        b1*cpoints[1][0] +
        b2*cpoints[2][0] +
        b3*cpoints[3][0] +
        b4*cpoints[4][0] +
        b5*cpoints[5][0];

    y = b0*cpoints[0][1] +
        b1*cpoints[1][1] +
        b2*cpoints[2][1] +
        b3*cpoints[3][1] +
        b4*cpoints[4][1] +
        b5*cpoints[5][1];

    z = b0*cpoints[0][2] +
        b1*cpoints[1][2] +
        b2*cpoints[2][2] +
        b3*cpoints[3][2] +
        b4*cpoints[4][2] +
        b5*cpoints[5][2];
}

void drawBitmapText(char * text, float x, float y, float z) 
{  
    char * c;
    glRasterPos3f(x, y, z);
    glColor3f(1.0f, 1.0f, 1.0f);
    for(c=text; c[0]!=0; ++c)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c[0]);
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

//    glMultMatrixf(m);
    return m;
}

void display() {

    //clear color & depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f * (float)degree;

    drawBitmapText(message, 2.0f, 3.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);                                                 // specify which matrix is the current matrix
    glLoadIdentity();                                                           // reset projection matrix

    gluLookAt(eyeX, eyeY, eyeZ,
              centerX, centerY, centerZ,
              upX, upY, upZ);

    glBegin(GL_LINES);
        glColor3f(1.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f);
        glColor3f(1.0f,0.0f,0.0f); glVertex3f(5.0f,0.0f,0.0f);
        glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f);
        glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.0f,5.0f,0.0f);
        glColor3f(0.0f,0.0f,1.0f); glVertex3f(0.0f,0.0f,0.0f);
        glColor3f(0.0f,0.0f,1.0f); glVertex3f(0.0f,0.0f,5.0f);
    glEnd();

    float angle = -360.0f/sec;
    myRotatef(M, angle, 0.0f,1.0f,0.0f);

    vec4f * curr_curves = new vec4f[npts];
    vec4f * next_curves = new vec4f[npts];
    for(size_t j=0; j<npts; ++j) {
        float u = (npts-j-1)*1.0/(npts-1);
        bezier(u, curr_curves[j].x, curr_curves[j].y, curr_curves[j].z);
    }

    vec4f curr_norm, next_norm;

    // use
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);
//    glUseProgram(m_program);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    for(size_t i=0; i<sec; ++i) {

        glBegin(GL_TRIANGLE_STRIP);
        for(size_t j=0; j<npts; ++j) {
            next_curves[j] = multiply(M, curr_curves[j]);
            if(j>0) {
                curr_norm = curr_curves[j] - curr_curves[j-1];
                curr_norm.normalize();
                next_norm = next_curves[j] - next_curves[j-1];
                next_norm.normalize();
            }
            else {
                curr_norm.x = cpoints[1][0]-cpoints[0][0];
                curr_norm.y = cpoints[1][1]-cpoints[0][1];
                curr_norm.z = cpoints[1][2]-cpoints[0][2];
                curr_norm.normalize();
                next_norm = multiply(M, curr_norm);
            }
            vec4f invec(-curr_curves[j].x, 0.0f, -curr_curves[j].z);
            vec4f outvec = curr_norm.cross(invec);
            curr_norm = curr_norm.cross(outvec);
            curr_norm.normalize();
            //
            vec4f invec2(-next_curves[j].x, 0.0f, -next_curves[j].z);
            vec4f outvec2 = next_norm.cross(invec2);
            next_norm = next_norm.cross(outvec2);
            next_norm.normalize();

            float u = j*1.0f/(npts-1);
            glColor3f(0.0f,1.0f,1.0f); glTexCoord2f((float)(i+0)/sec, u); glNormal3f(curr_norm.x,curr_norm.y,curr_norm.z); glVertex3f(curr_curves[j].x, curr_curves[j].y, curr_curves[j].z);
            glColor3f(0.0f,1.0f,1.0f); glTexCoord2f((float)(i+1)/sec, u); glNormal3f(next_norm.x,next_norm.y,next_norm.z); glVertex3f(next_curves[j].x, next_curves[j].y, next_curves[j].z);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
            glColor3f(1.0f,1.0f,1.0f); glNormal3f(0.0f,-1.0f,0.0f); glVertex3f(next_curves[npts-1].x, next_curves[npts-1].y, next_curves[npts-1].z);
            glColor3f(1.0f,1.0f,1.0f); glNormal3f(0.0f, -1.0f, 0.0f); glVertex3f(0.0f, cpoints[4][1], 0.0f);
            glColor3f(1.0f,1.0f,1.0f); glNormal3f(0.0f,-1.0f,0.0f); glVertex3f(curr_curves[npts-1].x, curr_curves[npts-1].y, curr_curves[npts-1].z);
        glEnd();

        swap(curr_curves, next_curves);
        myRotatef(M, angle, 0.0f,1.0f,0.0f);
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
//    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete [] curr_curves;
    delete [] next_curves;

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

    glViewport(0,0, w.width,w.height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat) w.width / w.height;
    gluPerspective(w.field_of_view_angle, aspect, w.z_near, w.z_far);
//    glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);

    glEnable(GL_DEPTH_TEST);
//    glShadeModel(GL_SMOOTH);
//    glDepthFunc(GL_LEQUAL);
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);                          // specify implementation-specific hints

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                                       // specify clear values for the color buffer
    glClearDepth(1.0f);                                                         // specify clear values for the depth buffer

    //XXX
    CBitmap image("obermaier.bmp");               //read bitmap image
    glGenTextures(1, &m_texture);               //allocate 1 texture
    glBindTexture(GL_TEXTURE_2D, m_texture);    //bind this texture to be active
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetBits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //specify minificaton filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //specify magnificaton filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);        //specify texture coordinate treatment
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);        //specify texture coordinate treatment

    glBindTexture(GL_TEXTURE_2D, 0);    //bind default texture to be active
}


void reshape(int width, int height) {
    w.width = width;
    w.height = height;
    glViewport(0,0, w.width,w.height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat aspect = (GLfloat) w.width / w.height;
    gluPerspective(w.field_of_view_angle, aspect, w.z_near, w.z_far);
//    glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
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
            degree += 1;
            npts = min(100, (int)npts+1);
            break;
        case '-':
            degree -= 1;
            npts = max(2, (int)npts-1);
            break;
        default:
            break;
    }
}

void zoomIn(){

}

void zoomOut(){

}

void mouseButton(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            printf("Down at (%d,%d)\n", x,y);
        }
        else {
            printf("Up at (%d,%d)\n", x,y);
        }
    }
    else if(button == 3)
        zoomIn();
    else if(button == 4)
        zoomOut();
    fflush(stdout);
}


//Allows manipulation of the camera
void changeCamera(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2, GLdouble x3, GLdouble y3, GLdouble z3){
    eyeX = x1; eyeY = y1; eyeZ = z1; 
    centerX = x2; centerY = y2; centerZ = z2; 
    upX = x3; upY = y3; upZ = z3;
}

//Translation functions
void moveLeft(){
    changeCamera(eyeX + .05, eyeY, eyeZ, centerX + 0.05, centerY, centerZ, upX, upY, upZ);
    display();
}

void moveRight(){
    changeCamera(eyeX - .05, eyeY, eyeZ, centerX - 0.05, centerY, centerZ, upX, upY, upZ);
    display();
}

void moveUp(){
    changeCamera(eyeX, eyeY - 0.05, eyeZ, centerX, centerY - 0.05, centerZ, upX, upY, upZ);
    display();
}

void moveDown(){
    changeCamera(eyeX, eyeY + 0.05, eyeZ, centerX, centerY + 0.05, centerZ, upX, upY, upZ);
    display();
}

void keyboard(int key, int x, int y){
    if(key == GLUT_KEY_UP)
        moveUp();
    else if(key == GLUT_KEY_DOWN)
        moveDown();    
    else if(key == GLUT_KEY_RIGHT)
        moveRight();    
    else if(key == GLUT_KEY_LEFT)
        moveLeft();   
}

void rotateShape(){
    changeCamera(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
    display();
}


void mouseMotion(int x, int y) {
    printf("x %d y %d\n", x,y);
    fflush(stdout);
    mouseX = x;
    mouseY = y;
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
    glutSpecialFunc(keyboard);
    glutDisplayFunc(display);                                   // register `Display Function'
    glutIdleFunc(display);                                      // register `Idle Function'
    glutReshapeFunc(reshape);                                   // register `Reshape Function'
    glutKeyboardFunc(keyboard);                                 // register `Keyboard Handler'
    glutMouseFunc(mouseButton);                                 // register `Mouse Button Handler'
    glutMotionFunc(mouseMotion);                                // register `Mouse Motion Handler'

    initialize();
    glutMainLoop();                                             // run GLUT mainloop

    return EXIT_SUCCESS;
}

