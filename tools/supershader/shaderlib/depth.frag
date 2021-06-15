#version 450

out vec4 frag_color;

#include "includes/depth_util.glsl"

void main() {             
    // sokol and webgl 1 do not support using the depth map as texture
    // gl_FragCoord.z is in [0,1] range
    frag_color = encodeDepth(gl_FragCoord.z);
}