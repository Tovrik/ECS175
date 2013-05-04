
// GLSL provides :
// attribute vec4 gl_MultiTexCoord0;
// gl_TexCoord is a `varying' variable

void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    // Set the position of the current vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
