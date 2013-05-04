CXX=g++
CXXFLAGS=-Wall -O2
LDLIBS=-lGL -lGLU -lglut -lGLEW
all: texture

texture: texture.cpp shader_utils.o bitmap/bitmap.h
	$(CXX) $(CXXFLAGS) texture.cpp shader_utils.o $(LDLIBS) -o texture

shader_util.o: shader_utils.cpp shader_utils.hpp
	$(CXX) $(CXXFLAGS) -c shader_utils.cpp

clean:
	rm -f *.o texture
