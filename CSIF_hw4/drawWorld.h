#ifndef DRAW_WORLD
#define DRAW_WORLD



class Mesh;

void drawScene(void);
int init_resources(void);
void free_resources(void);

void moveCamera(float);
void rotateCamera(float, char);
void strafeCamera(float, char);

void setStartCurr(int x, int y);
void setCurr(int x, int y);

void drawCube();

void updateTurtleMatrix(Mesh& obj3D, float angle, bool aroundOBJorigin);
void moveObject(Mesh&,float, float);
Mesh& getOBJ(char*);

// mouse
void initMouseStartPoint(int x, int y);
void updateCameraView(int x, int y);
#endif