#version 450

uniform u_vs_depthParams {
    mat4 lightMVPMatrix;
} depthParams;

in vec3 a_position;

void main() {
    gl_Position = depthParams.lightMVPMatrix * vec4(a_position, 1.0);
}