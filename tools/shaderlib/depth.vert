#version 450

uniform u_vs_depthParams {
    mat4 lightMVPMatrix;
} depthParams;

in vec3 a_position;
out vec2 v_projZW;

void main() {
    gl_Position = depthParams.lightMVPMatrix * vec4(a_position, 1.0);
    v_projZW = gl_Position.zw;
    #ifndef IS_GLSL
        gl_Position.y = -gl_Position.y;
    #endif
}