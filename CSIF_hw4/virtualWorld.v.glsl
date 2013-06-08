attribute vec4 coord3d; // rm_Vertex;
attribute vec3 v_normal; 

attribute vec2 texcoord;
varying vec2 f_texcoord;
// Projection matrix
uniform mat4 m_proj;
uniform bool textureON;
uniform vec3 f_color;
varying vec3 frag_color;
uniform vec4 lightPos;
uniform mat4 turtleMatrix;

struct lightSource
{
    vec4 position;
    vec4 diffuse;
	vec4 specular;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
    float spotCutoff, spotExponent;
    vec3 spotDirection;
};
lightSource light3 = lightSource(
    vec4(0.0,  1.25,  0.0, 1.0),
    vec4(0.5946,  0.9115,  0.9808, 1.0),
	vec4(1.0,  1.0,  1.0, 1.0),
    0.0, 1.0, 0.0,
    180.0, 0.0,
    vec3(0.0, 0.0, 0.0) );

lightSource light0 = lightSource(
  vec4(0.0,  1.0,  2.0, 1.0),
  vec4(1.0,  1.0,  1.0, 1.0),
  vec4(1.0,  1.0,  1.0, 1.0),
  0.0, 1.0, 0.0,
  180.0, 0.0,
  vec3(0.0, 0.0, 0.0)
);

lightSource light1 = lightSource(
    vec4(0.0, -2.0,  0.0, 1.0),
    vec4(2.0,  0.0,  0.0, 1.0),
    vec4(0.1,  0.1,  0.1, 1.0),
    0.0, 1.0, 0.0,
    80.0, 10.0,
    vec3(0.0, 1.0, 0.0)
);
lightSource lights[2];

vec4 scene_ambient = vec4(0.2, 0.2, 0.2, 1.0);

struct material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
material mymaterial = material(
  vec4(0.2, 0.2, 0.2, 1.0),
  vec4(1.0, 0.8, 0.8, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  5.0
);

material frontMaterial = material(
  vec4(0.2, 0.2, 0.2, 1.0),
  vec4(1.0, 0.8, 0.8, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  5.0
);

void main(void) {      
//     vec3 normal = normalize(v_normal);
    if(!textureON){//we are drawing an object
		lights[0] = light0;
		lights[1] = light1;
		
        vec3 light = normalize(vec3(1.0,1.0,1.0));
        vec3 normal = mat3(turtleMatrix) * v_normal;//vec3(turtleMatrix */* <- put this back*/ vec4(v_normal,1.0));
        // float brightness = dot(normal, light);
        
        // light0.position = lightPos;
        // vec3 lightDirection;
        // float attenuation;
        // vec3 vertexToLightSource = vec3(light0.position - coord3d/*vec4(normal, 1.0)*/);
        // float distance = length(vertexToLightSource);
        // lightDirection = normalize(vertexToLightSource);
        // attenuation = 1.0 / (light0.constantAttenuation
        //                      + light0.linearAttenuation * distance
        //                      + light0.quadraticAttenuation * distance * distance);
        // 
        // if (light0.spotCutoff <= 90.0) // spotlight
        // {
        //     float clampedCosine = max(0.0, dot(-lightDirection, normalize(light0.spotDirection)));
        //     if (clampedCosine < cos(light0.spotCutoff * 3.14159 / 180.0)) // outside of spotlight cone
        //     {
        //         attenuation = 0.0;
        //     }
        //     else
        //     {
        //         attenuation = attenuation * pow(clampedCosine, light0.spotExponent);
        //     }
        // }
        // 
        // vec3 diffuseReflection = attenuation
        // * vec3(light0.diffuse) * vec3(f_color)
        // * max(0.0, dot(normalize(normal), lightDirection));
        
        //    vec4 color = brightness * mymaterial;
        //    f_color = color + vec4(diffuseReflection, 1.0);
        
        // vec3 tempcolor = brightness * f_color;
        
        // frag_color = tempcolor + diffuseReflection;
		
		// initialize total lighting with ambient lighting
		vec3 totalLighting = vec3(scene_ambient) * vec3(frontMaterial.ambient);
		
		// vec3 lightDirection = normalize(vec3(-1.0, 1.0, 2.0));
		vec3 lightDirection = normalize(vec3(-1.0, 20.0, 2.0));
		vec3 diffuseReflection = light * max(0.0, dot(normal, lightDirection));
		
		vec3 viewDirection = normalize(vec3(0.0,0.0,0.0) - vec3(turtleMatrix * coord3d));
		
		vec3 specularReflection;
	  	if (dot(v_normal, lightDirection) < 0.0) // light source on the wrong side?
	    {
	      specularReflection = vec3(0.1, 0.1, 0.1); // no specular reflection
			totalLighting = totalLighting + specularReflection;
	    }
	  	else // light source on the right side
	    {
		  for(int i = 0; i < 2; i++)
		  {
		      specularReflection = vec3(lights[i].specular) * vec3(mymaterial.specular)
		        * pow(max(0.0, dot(reflect(-lightDirection, normalize(normal)), viewDirection)),
		              mymaterial.shininess);
			  totalLighting = totalLighting + diffuseReflection + specularReflection;
		  }
	    }
		// vec4 getz = vec4(0.0,0.0,1.0,0.0);
		frag_color = totalLighting;
        gl_Position  = m_proj * turtleMatrix * coord3d;//vec4(coord3d,1.0);
    }else {
         // multiply matrix times position vector
        gl_Position  = m_proj  * coord3d;//vec4(coord3d,1.0);
    }
    f_texcoord = texcoord;
}

