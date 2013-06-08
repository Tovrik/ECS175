#ifndef VirtualWorld_Globals_h
#define VirtualWorld_Globals_h
#include "readObj.h"


//MATRICES
glm::mat4 projMatrix; // Current total transform

glm::mat4 turtleMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
                                   glm::vec4(0.0, 1.0, 0.0, 0.0),
                                   glm::vec4(0.0, 0.0, 1.0, 0.0),
                                   glm::vec4(0.0, 0.0, 0.0, 1.0));

glm::mat4 identityMatrix  =   glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
                                        glm::vec4(0.0, 1.0, 0.0, 0.0),
                                        glm::vec4(0.0, 0.0, 1.0, 0.0),
                                        glm::vec4(0.0, 0.0, 0.0, 1.0));
// move cube into box centered at (0,0,-5)
glm::mat4 view  = glm::mat4( glm::vec4(1.0, 0.0, 0.0, 0.0),
                            glm::vec4(0.0, 1.0, 0.0, 0.0),
                            glm::vec4(0.0, 0.0, 1.0, 0.0),
                            glm::vec4(0.0, 0.0, -5.0, 1.0));
// n = near plane = -3
// f = far plane = 21*(-3) = -63
//glm::mat4 proj = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
//                           glm::vec4(0.0, 1.0, 0.0, 0.0),
//                           glm::vec4(0.0, 0.0, -22.0/(20*3), -0.33),
//                           glm::vec4(0.0, 0.0, -2.0*21/20, 0.0));

//n = -7
//f =  -7*k where k = 14 = 98
float n = 2, k = 150.0, f = n*k;
//
glm::mat4 proj = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
                           glm::vec4(0.0, 1.0, 0.0, 0.0),
                           glm::vec4(0.0, 0.0, -(k+1)/((k-1)*n), -1/n),
                           glm::vec4(0.0, 0.0, -(2*k)/(k-1), 0.0));

#endif
