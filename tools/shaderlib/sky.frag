#version 450

out vec4 frag_color;

in vec3 uv;

uniform samplerCube u_skyTexture;

void main(){    
    frag_color = texture(u_skyTexture, uv);
}