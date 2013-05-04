ECS 175: Computer Graphics
UC Davis, Spring 2013
Demo 5

Requirements:
GLUT
OpenGL
C++
cmake (unless you want to create your Makefile some other way)



Commands to create makefile + compile + run:

cmake .
make
./demo5.x

Additional options:
If libraries are installed in custom folders (e.g., your home folder), you may want to run cmake with the -D CMAKE_PREFIX_PATH=/yourpath/ option.

Demo Features:
-defines a parametric curve (circle)
-samples and renders parametric curve
-uses VBO without shaders
