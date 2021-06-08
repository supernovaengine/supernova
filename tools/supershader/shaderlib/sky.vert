#version 450

in vec3 a_position;

out vec3 uv;

uniform u_vsSkyParams {
    mat4 u_vpMatrix;

};

void main(){
    uv = a_position;
    vec4 pos = u_vpMatrix * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
} 
