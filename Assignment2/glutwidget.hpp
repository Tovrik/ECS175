/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Stefan Peterson
 SID#: 998556530 
 Assignment 2
 
*/
#ifndef _GLUTWIDGET_HPP_
#define _GLUTWIDGET_HPP_

#include "animal.hpp"

/*
 GLUT widget singleton
 */
class glutWidget
{
private:
    static const unsigned int m_width = 500;
    static const unsigned int m_height = 500;
    static void render();
    static void update();
    static void keyUp(unsigned char key, int, int);
    static void stop();
    static void initOpenGL();
    static void checkExtensions();
    static Animal *c;
    glutWidget(int argc, char **argv);
    
public:
    static glutWidget& init(int argc, char **argv)
    {
        static glutWidget gw(argc, argv);
        return gw;
    }
    static void run();
};



#endif

