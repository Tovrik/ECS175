/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include </usr/include/GL/glew.h>
#include <glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "readObj.h"

using namespace std;


void load_obj(const char* filename, Mesh* mesh) {
  ifstream in(filename, ios::in);
  if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }
  vector<int> nb_seen;

  string line;
    
    float minX = (float)INT_MAX, maxX = (float)INT_MIN, minY = (float)INT_MAX, maxY = (float)INT_MIN,
    minZ = (float)INT_MAX, maxZ = (float)INT_MIN;
    
//    printf("minx %.2f; minY %.2f; maxX %.2f; maxY %.2f\n", minX, minY, maxX, maxY);
  while (getline(in, line)) {
    if (line.substr(0,2) == "v ") {
      istringstream s(line.substr(2));
      glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0;
      mesh->vertices.push_back(v);
        
        if (v.y > mesh->maxVertexY.y)//special for light
            mesh->maxVertexY = v;
        
        if (v.x > maxX)//X
            maxX = v.x;
        if (v.x < minX)
            minX = v.x;
        
        if (v.y > maxY) //Y
            maxY = v.y;
        if (v.y < minY) 
            minY = v.y;
        
        if (v.z > maxZ)//Z
            maxZ = v.z;
        if (v.z < minZ)
            minZ = v.z;

    }  else if (line.substr(0,2) == "f ") {
      istringstream s(line.substr(2));
        GLuint a,b,c;//,d;
      s >> a; s >> b; s >> c; //s >> d;
        a--; b--; c--; //d--;
      mesh->elements.push_back(a); mesh->elements.push_back(b); 
        mesh->elements.push_back(c); //mesh->elements.push_back(d);
    }
  }

    
    
    float midx, midy, midz;
    midx = (maxX + minX) / 2.0;
    midy = (maxY + minY) / 2.0;
    midz = (maxZ + minZ) / 2.0;
    
    mesh->midPoint = glm::vec3(midx, midy, midz);

  mesh->normals.resize(mesh->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
  nb_seen.resize(mesh->vertices.size(), 0);
  for (unsigned int i = 0; i < mesh->elements.size(); i+=3) {
    GLuint ia = mesh->elements[i];
    GLuint ib = mesh->elements[i+1];
    GLuint ic = mesh->elements[i+2];
    glm::vec3 normal = glm::normalize(glm::cross(
      glm::vec3(mesh->vertices[ib]) - glm::vec3(mesh->vertices[ia]),
      glm::vec3(mesh->vertices[ic]) - glm::vec3(mesh->vertices[ia])));

    int v[3];  v[0] = ia;  v[1] = ib;  v[2] = ic;
    for (int j = 0; j < 3; j++) {
      GLuint cur_v = v[j];
      nb_seen[cur_v]++;
      if (nb_seen[cur_v] == 1) {
	mesh->normals[cur_v] = normal;
      } else {
	// average
	mesh->normals[cur_v].x = mesh->normals[cur_v].x * (1.0 - 1.0/nb_seen[cur_v]) + normal.x * 1.0/nb_seen[cur_v];
	mesh->normals[cur_v].y = mesh->normals[cur_v].y * (1.0 - 1.0/nb_seen[cur_v]) + normal.y * 1.0/nb_seen[cur_v];
	mesh->normals[cur_v].z = mesh->normals[cur_v].z * (1.0 - 1.0/nb_seen[cur_v]) + normal.z * 1.0/nb_seen[cur_v];
	mesh->normals[cur_v] = glm::normalize(mesh->normals[cur_v]);
      }
    }
  }
}


/*
void getObject(void) {
  load_obj("hippo.obj", &main_object);
}
*/
