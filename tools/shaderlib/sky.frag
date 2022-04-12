#version 450

out vec4 frag_color;

in vec3 uv;

uniform samplerCube u_skyTexture;

uniform u_fs_skyParams {
    vec4 color; //sRGB
} skyParams;

void main(){
    frag_color = skyParams.color * texture(u_skyTexture, uv);
}