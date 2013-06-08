
// Mesh class
// vertices are a vector of 4D points (x,y,z,1)
// normals are a vector of 3D point
// but the elements (triangles) is a vector of 3m integer indices,
// three per triangle. 

#ifndef READ_OBJ
#define READ_OBJ
#include <vector>
class Mesh {

public:
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> elements;
    glm::vec3 midPoint;
    
    glm::vec4 maxVertexY;//for lighting
    
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint ibo_elements;
    glm::mat4 turtle;
    // Creation and destruction
    Mesh(){
        turtle = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
                           glm::vec4(0.0, 1.0, 0.0, 0.0),
                           glm::vec4(0.0, 0.0, 1.0, 0.0),
                           glm::vec4(0.0, 0.0, 0.0, 1.0));
    }
    
    ~Mesh() {}
};


// Function to read in object in .obj format
void getObject(void);
void load_obj(const char*, Mesh*);
#endif
