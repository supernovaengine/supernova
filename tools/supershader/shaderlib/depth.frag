#version 450

in vec2 projZW;
out vec4 frag_color;

vec4 encodeDepth(float v) {
    vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc = fract(enc);
    enc -= enc.yzww * vec4(1.0/255.0,1.0/255.0,1.0/255.0,0.0);
    return enc;
}

void main() {             
    // sokol and webgl 1 do not support using the depth map as texture
    // so instead we write the depth value to the color map
    //frag_color = encodeDepth(gl_FragCoord.z);
    float depth = projZW.x / projZW.y;
    frag_color = encodeDepth(depth);
    //frag_color = vec4(vec3(decodeDepth(encodeDepth(gl_FragCoord.z)))/5.0, 1.0);
}