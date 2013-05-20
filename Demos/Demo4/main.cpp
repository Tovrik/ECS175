/*
 
 ECS 175: Computer Graphics
 UC Davis, Spring 2013
 
 Demo 4
 
 */
#include <iostream>
#include "glutwidget.hpp"


/*
    Main function: Program entry point
 */
int main (int argc, char** argv)
{
    glutWidget gluw = glutWidget::init(argc,argv);
    gluw.run();
    
    return 0;
}