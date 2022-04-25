#version 450

uniform u_vs_depthParams {
    mat4 lightMVPMatrix;
} depthParams;

in vec3 a_position;
out vec2 v_projZW;

#ifdef HAS_SKINNING
    in vec4 a_boneWeights;
    in vec4 a_boneIds;

    uniform u_vs_skinning {
        mat4 bonesMatrix[MAX_BONES];
    };
#endif

void main() {

    vec3 pos = a_position;

    #ifdef HAS_SKINNING
        mat4 boneTransform = mat4(0.0);
        //sokol send boneIds (USHORT4N) normalized, needed "expand" the normalized vertex shader
        boneTransform += bonesMatrix[int(a_boneIds[0] * 65535.0)] * a_boneWeights[0];
        boneTransform += bonesMatrix[int(a_boneIds[1] * 65535.0)] * a_boneWeights[1];
        boneTransform += bonesMatrix[int(a_boneIds[2] * 65535.0)] * a_boneWeights[2];
        boneTransform += bonesMatrix[int(a_boneIds[3] * 65535.0)] * a_boneWeights[3];

        vec4 skinVertex = vec4(pos, 1.0);
        skinVertex = boneTransform * skinVertex;
        pos = vec3(skinVertex) / skinVertex.w;
    #endif

    gl_Position = depthParams.lightMVPMatrix * vec4(pos, 1.0);
    v_projZW = gl_Position.zw;
    #ifndef IS_GLSL
        gl_Position.y = -gl_Position.y;
    #endif
}