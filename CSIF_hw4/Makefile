CC = g++
CFLAGS = -g -DGL_GLEXT_PROTOTYPES 
INCLUDES = -I/usr/include/GL
LDFLAGS = -lglut -lGLU -lGL -lX11 -lGLEW

RM = /bin/rm -f 

virtualWorld:  main.o drawWorld.o shader_utils.o readObj.o readBMP.o
	$(CC) $(CFLAGS) -o virtualWorld main.o shader_utils.o drawWorld.o readObj.o readBMP.o $(LDFLAGS) 
%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
clean: 
	$(RM) *.o
