#version 450

uniform u_vs_depthParams {
    mat4 mvpMatrix;
} depthParams;

in vec3 a_position;

void main() {
    gl_Position = depthParams.mvpMatrix * vec4(a_position, 1.0);
}