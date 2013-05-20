#version 110
 
varying  vec3 ex_Normal; //interpolated normals as computed in vertex shader (a varying attribute is passed from vertex shader)
 
//fragment shaders operate on "all" active fragments in parallel (i.e., all fragments that are being drawn while the shader is active)
void main(void)	
{
	float d = abs(dot(ex_Normal,vec3(0.0,0.0,-0.8))); 	//basic, fake lighting based on dot product of normal and arbitrary "light vector"
        gl_FragColor = vec4(d,d,d,1.0);				//sets color of current fragment (you can set this color to anything else)
}