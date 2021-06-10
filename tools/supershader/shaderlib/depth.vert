#version 450

uniform u_vs_depthParams {
    mat4 mvpMatrix;
} depthParams;

in vec3 a_position;
out vec2 projZW;

void main() {
    gl_Position = depthParams.mvpMatrix * vec4(a_position, 1.0);
    projZW = gl_Position.zw;
}