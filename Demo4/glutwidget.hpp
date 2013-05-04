/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 4
 
 */
#ifndef _GLUTWIDGET_HPP_
#define _GLUTWIDGET_HPP_

class Cube;

/*
 GLUT widget singleton
 */
class glutWidget
{
private:
    static Cube *c;
    static unsigned int m_program;                 //shader program
    static unsigned int m_vertexsh;                //vertex shader
    static unsigned int m_fragmentsh;              //fragment shader
    static int m_pos_attribute_location;    //points to attribute location in vertex shader
    static int m_normal_attribute_location; //points to attribute location in vertex shader
    static unsigned int m_frame;
    static const unsigned int m_width = 500;
    static const unsigned int m_height = 500;
    static void render();
    static void update();
    static void keyUp(unsigned char key, int, int);
    static void stop();
    static void initOpenGL();
    static void drawCubes();
    static void makeShaders();
    glutWidget(int argc, char **argv);
    static void checkExtensions();
public:
    static glutWidget& init(int argc, char **argv)
    {
        static glutWidget gw(argc, argv);
        return gw;
    }
    static void run();
};





#endif

