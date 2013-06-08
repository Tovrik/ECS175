#include "drawWorld.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_utils.h"
#include "Globals.h"
#include "readObj.h"
#include "readBMP.h"
//#include "Globals.h"
#define PI 3.14159265

enum {Attribute = 0, Uniform};

GLuint program;
GLint attribute_coord3d;
GLint attribute_texcoord;
GLint uniform_color;
GLint uniform_proj_matrix;  // pointer to uniform variable - total matrix
GLint uniform_texture;
GLint attribute_normal;
GLint texture_On;
GLint uniform_LightPos;
GLint uniform_turtleMat;

/* GLOBAL VARAIBLES */
/* (storage is actually allocated here) */
int screen_W=1200;  /* window width */
int screen_H=1200;  /* window height */


int start_mx, start_my, cur_mx, cur_my; 
//GLint attribute_normal;
Image texImage;
Image brickTexture;
Image lightBlueTexture;
GLuint brickTextureId;
GLuint lightBlueTextureId;
GLuint circuitTextureId;
GLuint wallAdsTextureId;


GLuint textureId;
Mesh tronCycle;
Mesh recognizer;
Mesh recognizer2;
Mesh recognizer3;
Mesh recognizer4;
Mesh recognizer5;


Mesh spaceShip1;
Mesh spaceShip2;
Mesh shipFleet;
Mesh satelite;
Mesh building;


Mesh& getOBJ(char* str)
{
    if (strcmp(str, "tron") == 0) 
        return tronCycle;
    else if (strcmp(str, "recognizer") == 0)
        return recognizer;
    else if (strcmp(str, "recognizer2") == 0)
        return recognizer2;
    else if (strcmp(str, "shipFleet") == 0)
        return shipFleet;
    else if (strcmp(str, "recognizer3") == 0)
        return recognizer3;
    else if (strcmp(str, "recognizer4") == 0)
        return recognizer4;
    else if (strcmp(str, "recognizer5") == 0)
        return recognizer5;
    else if (strcmp(str, "satelite") == 0) 
        return satelite;
    else 
        printf("cannot load object: %s\n", str);
}


// mouse
glm::vec2 mouseStartPoint;
glm::vec2 mouseEndPoint;
float rotateRatio = 1 * PI / 180;
glm::mat4 spaceRotationMat = glm::mat4(1.0f);

//GLuint vbo_mesh_vertices;
//GLuint vbo_mesh_normals;
//GLuint ibo_mesh_elements;

using namespace std;
void bindVariable(GLint &variable, char *variableName, int type)
{
    if (type == Attribute) 
        variable = glGetAttribLocation(program, variableName);
    else if (type == Uniform)
        variable = glGetUniformLocation(program, variableName);
    
    if (variable == -1) {
        fprintf(stderr, "Could not bind %s\n", variableName);
    }
//    printf("%s location in gpu %d\n", variableName,variable);
}

void setStartCurr(int x, int y)
{
    start_mx = cur_mx = x;
    start_my = cur_my = y;
}

void setCurr(int x, int y)
{
    cur_mx = x;
    cur_my = y;
}

void moveCamera(float move) 
{
    view[3].z += move;
}

void strafeCamera(float len, char dir)
{
    if (dir == 'x')
        view[3].x += len;
    else if (dir == 'y') 
        view[3].y += len;

//    printf("(%.2f, %.2f)\n",view[3].x, view[3].y);
}

void rotateCamera(float angle, char axis)
{
    glm::vec3 axis_in_object_coord(0.0,0.0,0.0);
    
    if (axis == 'y') //lloking left or right
        axis_in_object_coord.y = angle < 0 ? -1.0 : 1.0;
    else if (axis == 'x') // looking up or down
        axis_in_object_coord.x = angle < 0 ? 1.0 : -1.0;
    
    angle = fabsf(angle);
    glm::mat4 tempMat = glm::rotate(identityMatrix, glm::degrees(angle), axis_in_object_coord);
    
    view = tempMat * view;   
//    printMat(view);
//    printf("view (%.2f, %.2f, %.2f)", view[3].x, view[3].y, view[3].z);   
}

void printMat(glm::mat4 myMatrix)
{
    printf("matrix:\n");
    int i,j;
    for (j=0; j<4; j++){
        for (i=0; i<4; i++){
            printf("i:%d j:%d %f ",i,j,myMatrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
}

void moveObject(Mesh& obj3D, float speed, float resetValue)
{
//    printMat(obj3D.turtle);
    if (obj3D.turtle[3][2] >= 210.0)
        obj3D.turtle[3][2] -= resetValue;

    obj3D.turtle = glm::translate(obj3D.turtle, glm::vec3(0,0,speed));
}


void updateTurtleMatrix(Mesh& obj3D, float angle, bool aroundOBJorigin)
{
    glm::vec3 axis_in_camera_coord = glm::vec3(0,1,0);

    glm::mat3 camera2object = glm::inverse(glm::mat3(obj3D.turtle) * glm::mat3(identityMatrix/*should be view*/));
    glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
    
    //translate to origin
//    printf("object midpoint (%.2f, %.2f, %.2f)\n",
//           obj3D.midPoint.x, obj3D.midPoint.y,obj3D.midPoint.z);
    
    if (aroundOBJorigin) 
       obj3D.turtle= glm::translate(obj3D.turtle, obj3D.midPoint);
    //rotate
    obj3D.turtle = glm::rotate(obj3D.turtle, glm::degrees(angle), axis_in_object_coord);

    if (aroundOBJorigin) 
        obj3D.turtle = glm::translate(obj3D.turtle, -obj3D.midPoint);
    
//    printMat(obj3D.turtle);
    //translate back
//    obj3D.turtle = glm::translate(obj3D.turtle, obj3D.midPoint);

}

void drawCube()
{
    
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity();
// 	glTranslatef(0.0f, -1.0f, 0.0f);
	glTranslatef(2.0f, 1.0f, 0.0f);
    glMaterialf(GL_FRONT, GL_EMISSION, 12.0);
    glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.75f, 0.0f);
	
	//Pentagon
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	
//    float x, y, z;
//    glColor3f(1.0, 1.0, 1.0);
//    
//    for (int i=0; i < object3D.elements.size(); i++) {
//        unsigned int pos =  object3D.elements[i];
//        x = object3D.vertices[pos].x;
//        y = object3D.vertices[pos].y;
//        z = object3D.vertices[pos].z;
//        glVertex3f(x, y, z);
//    }
    
    glEnd(); //End
    glPopMatrix();
}

float s=30,t=30;
float s1=1, t1=1;
GLfloat floorVertices[] = {
    -150, 0, 150, -s, t,
    150, 0, 150, s, t,
    150, 0, -150, s, -t,
    -150, 0, -150, -s, -t,
    
    4, 4, -20.0, -s1, -t1, 
    12, 4, -20.0, s1, -t1, 
    12, 12, -20.0, s1, t1, 
    4, 12, -20.0, -s1, t1,
    
    -12, 4, -20.0, -s1, -t1, 
    -4, 4, -20.0, s1, -t1, 
    -4, 12, -20.0, s1, t1, 
    -12, 12, -20.0, -s1, t1,
    
    -4, 4, 20.0, -s1, -t1, 
    4, 4, 20.0, s1, -t1, 
    4, 12, 20.0, s1, t1, 
    -4, 12, 20.0, -s1, t1,    
};

GLubyte floorElements[] = {0,1,2,3};

GLubyte wallElements[] = {
    4, 5, 6,
    6, 7, 4
};

GLubyte wallElements2[] = {
    8, 9, 10,
    10, 11, 8
};

GLubyte wallElements3[] = {
    12, 13, 14,
    14, 15, 12
};

void drawLowerGrid()
{


    glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
    glBindTexture(GL_TEXTURE_2D, textureId);
    //    glGenerateMipmap(textureId);
    // current 2D texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Two attributes for the vertex, position and texture coordinate
    // Let OpenGL know we'll use both of them. 
    glEnableVertexAttribArray(attribute_coord3d);
    glEnableVertexAttribArray(attribute_texcoord);
    
    
    // Describe the position attribute and where the data is in the array
    glVertexAttribPointer(
                          attribute_coord3d, // attribute ID
                          3,                 // number of elements per vertex, here (x,y,z)
                          GL_FLOAT,          // the type of each element
                          GL_FALSE,          // take our values as-is, don't normalize
                          5*sizeof(float),  // stride between one position and the next
                          floorVertices  // pointer to first position in the C array
                          );
    
    glVertexAttribPointer(
                          attribute_texcoord, // attribute
                          2,                  // number of elements per vertex, (s,t)
                          GL_FLOAT,           // the type of each element
                          GL_FALSE,           // take our values as-is
                          5*sizeof(float),    // stride to next texture element
                          floorVertices+3     // offset of first element
                          );
    
    // Send GPU projection matrix
    glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));
    glUniform1i(texture_On,true);
    glDisableVertexAttribArray(attribute_normal);
    glUniformMatrix4fv(uniform_turtleMat, 1, GL_FALSE, glm::value_ptr(identityMatrix));

//    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // draw the grid....
//    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, floorElements);
    // draw the grid....
    glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1i(uniform_texture, 2);
    
    //    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, floorElements);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, floorElements);
    
    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureId);	
	glUniform1i(uniform_texture, 2);
    //	// draw the wall....
    glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_BYTE, wallElements);
    
    
    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureId);	
	glUniform1i(uniform_texture, 2);
    //	// draw the wall....
    glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_BYTE, wallElements2);
    
    
    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wallAdsTextureId);	
	glUniform1i(uniform_texture, 1);
    //	// draw the wall....
    glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_BYTE, wallElements3);    
        
    glDisableVertexAttribArray(attribute_coord3d);
    glDisableVertexAttribArray(attribute_texcoord);

}

void draw3D_OBJ(Mesh obj3D, glm::vec3 color, GLfloat lightY)
{    
    
    glEnableVertexAttribArray(attribute_coord3d);
    glBindBuffer(GL_ARRAY_BUFFER, obj3D.vbo_vertices);
    glVertexAttribPointer(
                          attribute_coord3d,  // attribute
                          4,                  // number of elements per vertex, here (x,y,z,w)
                          GL_FLOAT,           // the type of each element
                          GL_FALSE,           // take our values as-is
                          0,                  // no extra data between each position
                          (void*)0                   // offset of first element
                          );
    
    glEnableVertexAttribArray(attribute_normal);
    glBindBuffer(GL_ARRAY_BUFFER, obj3D.vbo_normals);
    
    glVertexAttribPointer(
                          attribute_normal, // attribute
                          3,                  // number of elements per vertex, here (x,y,z)
                          GL_FLOAT,           // the type of each element
                          GL_FALSE,           // take our values as-is
                          0,                  // no extra data between each position
                          (void*)0                   // offset of first element
                          );
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj3D.ibo_elements);
    
    int buffer_size;  
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size); 
    //    printf("buffer size %d\n", buffer_size);
    glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));
    glUniform1i(texture_On,false);
    glUniform3f(uniform_color, color.x, color.y, color.z);
    //    printf("max vertex (%.2f, %.2f, %.2f)\n", tronCycle.maxVertex.x,object3D.maxVertex.y,object3D.maxVertex.z);
    glUniformMatrix4fv(uniform_turtleMat, 1, GL_FALSE, glm::value_ptr(obj3D.turtle));

    glUniform4f(uniform_LightPos, obj3D.maxVertexY.x, 
                obj3D.maxVertexY.y+lightY,obj3D.maxVertexY.z, 1.0);
    glDrawElements(GL_TRIANGLES, buffer_size/sizeof(GLuint), GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(attribute_coord3d);
    glDisableVertexAttribArray(attribute_texcoord);
    glDisableVertexAttribArray(attribute_normal);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

glm::mat4 tempView;
void pushMatrix()
{
    tempView = projMatrix;
}

void popMatrix()
{
    projMatrix = tempView;
}

// Draw the floor and wall
void drawScene(void) 
{
    glUseProgram(program);
    // Projection matrix
    projMatrix = proj * view;
    glUniformMatrix4fv(uniform_turtleMat, 1, GL_FALSE, glm::value_ptr(identityMatrix));

    drawLowerGrid();
//    drawUpperWall();
//    drawTRON();
    draw3D_OBJ(tronCycle, glm::vec3(0.125,0.125,0.125), 0.35);
     
    draw3D_OBJ(recognizer, glm::vec3(0.15,0.15,0.15), 0.50);
    draw3D_OBJ(recognizer2, glm::vec3(0.15,0.15,0.15), 0.50);
    draw3D_OBJ(recognizer3, glm::vec3(0.15,0.15,0.15), 0.50);
    draw3D_OBJ(recognizer4, glm::vec3(0.15,0.15,0.15), 0.50);
    draw3D_OBJ(recognizer5, glm::vec3(0.15,0.15,0.15), 0.50);

    draw3D_OBJ(spaceShip1, glm::vec3(0.15,0.18,0.15), 1.0);
//    draw3D_OBJ(spaceShip2, glm::vec3(0.15,0.18,0.15), 1.0);
    draw3D_OBJ(shipFleet, glm::vec3(0.20,0.20,0.20), 1.0);
    draw3D_OBJ(satelite, glm::vec3(0.80,0.8,0.8), 1.0);
//    draw3D_OBJ(building, glm::vec3(0.75,0.78,0.75), 1.0);


//    drawRecognizer();
//    drawCube();
}

void loadImageTexture(GLuint &imageID, char *fileName, bool genMipMap)
{
    Image image;
    int flag = ImageLoad(fileName, &image);
    if (flag != 1) {
        printf("Trouble reading \"%s\"\n", fileName);
    }
    
    if (strcmp(fileName, "brick.bmp") == 0) {
        printf("Hey, here is tron_bike.bmp\n");
        glActiveTexture(GL_TEXTURE1); // Load texture into GPU texture unit 0
        glGenTextures(1,&imageID); // Make a texture object
    } else if (strcmp(fileName, "TRON_Tile_moreGlow.bmp") == 0) {
        glActiveTexture(GL_TEXTURE2); // Load texture into GPU texture unit 0
        glGenTextures(2,&imageID); // Make a texture object        
    } else if (strcmp(fileName, "tron_m.bmp") == 0) {
        printf("Hello call tron_m\n");
        glActiveTexture(GL_TEXTURE1); // Load texture into GPU texture unit 0
        glGenTextures(1,&imageID); // Make a texture object        
    }
    else {
        printf("Here is not good\n");
        glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
        glGenTextures(1,&imageID); // Make a texture object
    }
    
    
//    glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
//    glGenTextures(1,&imageID); // Make a texture object
    glBindTexture(GL_TEXTURE_2D, imageID); // Use this object as the 
    
    if (genMipMap) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, // the current 2D texture
                          GL_RGB, //internal format for texture
                          image.sizeX, // size in s coord
                          image.sizeY, // size in t coord
                          GL_RGB, // incoming data format; should match internal
                          GL_UNSIGNED_BYTE, // type of incoming data
                          image.data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
                     0, // Mipmap level
                     GL_RGB, //internal format for texture
                     image.sizeX, // size in s coord
                     image.sizeY, // size in t coord
                     0, // should always be 0
                     GL_RGB, // incoming data format; should match internal
                     GL_UNSIGNED_BYTE, // type of incoming data
                     image.data // pointer to the data
                     ); 
    }
}

void loadObject(Mesh &object3D, char *filename, glm::vec3 trasnlateVector)
{
    load_obj(filename, &object3D);
    
    glGenBuffers(1, &object3D.vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, object3D.vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, object3D.vertices.size()*sizeof(object3D.vertices[0]), &object3D.vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &object3D.vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, object3D.vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, object3D.normals.size()*sizeof(object3D.normals[0]), &object3D.normals[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &object3D.ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object3D.ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, object3D.elements.size() * sizeof(object3D.elements[0]), 
                 &object3D.elements[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    object3D.turtle = glm::translate(object3D.turtle, trasnlateVector);

//    printf(" max vertex for %s  is : (%.2f, %.2f, %.2f) \n", filename, 
//           object3D.maxVertexY.x,object3D.maxVertexY.y,object3D.maxVertexY.z );
}

int init_resources()
{


    // Projection matrix
    projMatrix = proj * view;
    
    // Read in texture image
    loadImageTexture(textureId, (char*)"TRON_Tile_moreGlow.bmp", true);
    loadImageTexture(brickTextureId, (char*)"brick.bmp", true);
    loadImageTexture(wallAdsTextureId, (char*)"tron_m.bmp", true);
//    loadImageTexture(lightBlueTextureId, (char*)"lightBlue.bmp", false);
//    loadImageTexture(circuitTextureId, (char*)"seamless_circuitry_82b83f.bmp", true);

    // Error flag is initially false
    GLint link_ok = GL_FALSE;
    // Indices for vertex and fragment shaders
    GLuint vs, fs;
    
    // create_shader is a function in shader_utils that reads in 
    // a vertex or fragment program from a file, creates a shader 
    // object, puts the program into the object, and compiles it.
    // If all goes well, returns 1. 
    vs = create_shader("virtualWorld.v.glsl", GL_VERTEX_SHADER);
    if (vs == 0) return 0;
    
    fs = create_shader("virtualWorld.f.glsl", GL_FRAGMENT_SHADER);
    if (fs == 0) return 0;
    
    
    // The GPU program contains both the vertex and shader programs. 
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    
    // Link them together.
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        fprintf(stderr, "glLinkProgram: failed\n");
        return 0;
    }
    
    // Now figure out where the linker put everything
    // We don't know where the vertex program is going to store all its
    // input data until after linking. 
    // Results are indicies into some table on the GPU. 
    
    // Ask for the index of shader variable coord3d.
//    bindVariable(&attribute_coord3d, "coord3d");
    bindVariable(attribute_coord3d, (char*)"coord3d", Attribute); 
    bindVariable(attribute_normal, (char*)"v_normal", Attribute);
    bindVariable(attribute_texcoord, (char*)"texcoord", Attribute);
    bindVariable(uniform_color, (char*)"f_color", Uniform);
    bindVariable(uniform_texture, (char*)"u_texture", Uniform);
    bindVariable(uniform_proj_matrix, (char*)"m_proj", Uniform);
    bindVariable(texture_On, (char*)"textureON", Uniform);
    bindVariable(uniform_LightPos, (char*)"lightPos", Uniform);
    bindVariable(uniform_turtleMat, (char*)"turtleMatrix", Uniform);
    
    loadObject(tronCycle, (char*)"untitled.obj", glm::vec3(0, 0, 0));
    loadObject(recognizer, (char*)"medieval_church_norms.obj", glm::vec3(-10, 0, 0));

    loadObject(spaceShip1, (char*)"medieval_store_norms.obj", glm::vec3(25, 0, 0));
//    loadObject(spaceShip2, (char*)"spaceShip2.obj", glm::vec3(-65, 0, 0));
    loadObject(shipFleet, (char*)"shipFleet.obj", glm::vec3(0, -65, -400));
    loadObject(satelite, (char*)"satelite.obj", glm::vec3(-45, 75, -20));
//    loadObject(building, (char*)"building.obj", glm::vec3(0, 0, -200));
    recognizer2 = recognizer;
    recognizer2.turtle = glm::translate(recognizer2.turtle, glm::vec3(50, 0, 30));
    
    recognizer3 = recognizer;
    recognizer3.turtle = glm::translate(recognizer3.turtle, glm::vec3(80, 5, 130));
    
    recognizer4 = recognizer;    
    recognizer4.turtle = glm::translate(recognizer4.turtle, glm::vec3(-120, 0, -30));
    
    recognizer5 = recognizer;
    recognizer5.turtle = glm::translate(recognizer5.turtle, glm::vec3(-100, 0, 115));

    strafeCamera(-2.10, 'y');//up
//   recognizer.turtle= glm::translate(recognizer.turtle, -recognizer.midPoint);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    
    // If all went well....
    return 1;
}

void initMouseStartPoint(int x, int y)
{
    mouseStartPoint = glm::vec2(x, y);
}

void updateCameraView(int x, int y)
{
    mouseEndPoint = glm::vec2(x, y);
	
	// set up the correct cooridinate 
	glm::vec2 normalizePoint = mouseEndPoint - mouseStartPoint;
	normalizePoint[1] = -normalizePoint[1];
	glm::vec2 x_normal = glm::vec2(1, 0);
	
	float length = sqrt(pow(normalizePoint[0], 2) + pow(normalizePoint[1], 2));
	normalizePoint = normalizePoint / length;
	
	float result = acos(normalizePoint[0]) * 180 / PI;
	float alpha = 0;
    
	glm::mat4 rotateMat;
	glm::mat4 midMat;
	glm::mat4 rotateBackMat;
	if(normalizePoint[1] >= 0)
	{
		if(result >= 0 && result <= 90)
		{
			alpha = (90 - result) * PI / 180;
			rotateMat = glm::mat4(
                                  glm::vec4(cos(alpha), sin(alpha), 0.0, 0.0),
                                  glm::vec4(-sin(alpha), cos(alpha), 0.0, 0.0),
                                  glm::vec4(0.0, 0.0, 1.0, 0.0),
                                  glm::vec4(0.0, 0.0, 0.0, 1.0)
                                  );				
			midMat = glm::mat4(
                               glm::vec4(1.0, 0.0, 0.0, 0.0),
                               glm::vec4(0.0, cos(rotateRatio), -sin(rotateRatio), 0.0),
                               glm::vec4(0.0, sin(rotateRatio), cos(rotateRatio), 0.0),
                               glm::vec4(0.0, 0.0, 0.0, 1.0)
                               );
			rotateBackMat = glm::mat4(
                                      glm::vec4(cos(alpha), -sin(alpha), 0.0, 0.0),
                                      glm::vec4(sin(alpha), cos(alpha), 0.0, 0.0),
                                      glm::vec4(0.0, 0.0, 1.0, 0.0),
                                      glm::vec4(0.0, 0.0, 0.0, 1.0)
                                      );
			// return rotateBackMat * midMat * rotateMat
		} else
		{	
			alpha = (result - 90) * PI / 180;
			rotateMat = glm::mat4(
                                  glm::vec4(cos(alpha), -sin(alpha), 0.0, 0.0),
                                  glm::vec4(sin(alpha), cos(alpha), 0.0, 0.0),
                                  glm::vec4(0.0, 0.0, 1.0, 0.0),
                                  glm::vec4(0.0, 0.0, 0.0, 1.0)
                                  );
			midMat = glm::mat4(
                               glm::vec4(1.0, 0.0, 0.0, 0.0),
                               glm::vec4(0.0, cos(rotateRatio), -sin(rotateRatio), 0.0),
                               glm::vec4(0.0, sin(rotateRatio), cos(rotateRatio), 0.0),
                               glm::vec4(0.0, 0.0, 0.0, 1.0)
                               );
			rotateBackMat = glm::mat4(
                                      glm::vec4(cos(alpha), sin(alpha), 0.0, 0.0),
                                      glm::vec4(-sin(alpha), cos(alpha), 0.0, 0.0),
                                      glm::vec4(0.0, 0.0, 1.0, 0.0),
                                      glm::vec4(0.0, 0.0, 0.0, 1.0)
                                      );
			// return rotateBackMat * midMat * rotateMat;
		}
	} else if(normalizePoint[1] < 0)
	{
		if(result >= 0 && result <= 90)
		{
			alpha = (90 - result) * PI / 180;
			rotateMat = glm::mat4(
                                  glm::vec4(cos(alpha), -sin(alpha), 0.0, 0.0),
                                  glm::vec4(sin(alpha), cos(alpha), 0.0, 0.0),
                                  glm::vec4(0.0, 0.0, 1.0, 0.0),
                                  glm::vec4(0.0, 0.0, 0.0, 1.0)
                                  );
			midMat = glm::mat4(
                               glm::vec4(1.0, 0.0, 0.0, 0.0),
                               glm::vec4(0.0, cos(rotateRatio), sin(rotateRatio), 0.0),
                               glm::vec4(0.0, -sin(rotateRatio), cos(rotateRatio), 0.0),
                               glm::vec4(0.0, 0.0, 0.0, 1.0)
                               );
			rotateBackMat = glm::mat4(
                                      glm::vec4(cos(alpha), sin(alpha), 0.0, 0.0),
                                      glm::vec4(-sin(alpha), cos(alpha), 0.0, 0.0),
                                      glm::vec4(0.0, 0.0, 1.0, 0.0),
                                      glm::vec4(0.0, 0.0, 0.0, 1.0)
                                      );
			// return rotateBackMat * midMat * rotateMat;
		}
		else
		{
			alpha = (result - 90) * PI / 180;
			rotateMat = glm::mat4(
                                  glm::vec4(cos(alpha), sin(alpha), 0.0, 0.0),
                                  glm::vec4(-sin(alpha), cos(alpha), 0.0, 0.0),
                                  glm::vec4(0.0, 0.0, 1.0, 0.0),
                                  glm::vec4(0.0, 0.0, 0.0, 1.0)
                                  );
			midMat = glm::mat4(
                               glm::vec4(1.0, 0.0, 0.0, 0.0),
                               glm::vec4(0.0, cos(rotateRatio), sin(rotateRatio), 0.0),
                               glm::vec4(0.0, -sin(rotateRatio), cos(rotateRatio), 0.0),
                               glm::vec4(0.0, 0.0, 0.0, 1.0)
                               );
			rotateBackMat = glm::mat4(
                                      glm::vec4(cos(alpha), -sin(alpha), 0.0, 0.0),
                                      glm::vec4(sin(alpha), cos(alpha), 0.0, 0.0),
                                      glm::vec4(0.0, 0.0, 1.0, 0.0),
                                      glm::vec4(0.0, 0.0, 0.0, 1.0)
                                      );
			// return rotateBackMat * midMat * rotateRatio;
		}
	} else {
		rotateMat = midMat = rotateBackMat = glm::mat4(1.0f);
	}
    
    mouseStartPoint[0] = x;
	mouseStartPoint[1] = y;
	spaceRotationMat = rotateBackMat * midMat * rotateMat;
	view =  spaceRotationMat * view;
}

void free_resources()
{
    glDeleteProgram(program);
//    glDeleteTextures(1,&textureId);
}
