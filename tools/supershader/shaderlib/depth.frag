#version 450

out vec4 frag_color;
in vec2 v_projZW;

#include "includes/depth_util.glsl"

void main() {  
    // sokol and webgl 1 do not support using the depth map as texture

    // gl_FragCoord.z is in [0,1] range
    //frag_color = encodeDepth(gl_FragCoord.z);

    // Higher precision equivalent of gl_FragCoord.z in some platforms. See Three.js depth_frag.glsl.js
	frag_color = encodeDepth(0.5 * v_projZW[0] / v_projZW[1] + 0.5);
}