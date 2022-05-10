#version 450

#define DEPTH_SHADER

uniform u_vs_depthParams {
    mat4 lightMVPMatrix;
} depthParams;

in vec3 a_position;
out vec2 v_projZW;

#include "includes/skinning.glsl"
#include "includes/morphtarget.glsl"

void main() {

    vec3 pos = a_position;

    pos = getMorphPosition(pos);
    pos = getSkinPosition(pos, getBoneTransform());

    gl_Position = depthParams.lightMVPMatrix * vec4(pos, 1.0);
    v_projZW = gl_Position.zw;
    #ifndef IS_GLSL
        gl_Position.y = -gl_Position.y;
    #endif
}