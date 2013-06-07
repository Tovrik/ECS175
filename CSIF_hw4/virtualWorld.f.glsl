varying vec2 f_texcoord;
uniform sampler2D u_texture;
uniform bool textureON;
varying vec3 frag_color;

void main(void) {        

    if (textureON){
		gl_FragColor = texture2D(u_texture, f_texcoord);
        if (gl_FragColor == vec4(0,0,0,1)) {
            discard;
        }
		// gl_FragColor = (1 - phi) * texture2D(u_texture, f_texcoord) + phi * vec4(1.0, 1.0, 1.0, 1.0);
    }
    else {
        gl_FragColor = vec4(frag_color, 1.0);//vec4(0.2,0.8,0.3, 1.0);
    }
 
}

