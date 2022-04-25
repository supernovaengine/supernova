#version 450

uniform u_vs_pbrParams {
    mat4 modelMatrix;
    mat4 normalMatrix;
    mat4 mvpMatrix;
} pbrParams;

#ifdef HAS_TEXTURERECT
    uniform u_vs_spriteParams {
        vec4 textureRect;
    } spriteParams;
#endif

in vec3 a_position;
out vec3 v_position;

#ifdef HAS_NORMALS
    in vec3 a_normal;
#endif

#ifdef HAS_TANGENTS
    in vec4 a_tangent;
#endif

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
    out mat3 v_tbn;
#else
    out vec3 v_normal;
#endif
#endif

#ifdef HAS_UV_SET1
    in vec2 a_texcoord1;
#endif

#ifdef HAS_UV_SET2
    in vec2 a_texcoord2;
#endif

out vec2 v_uv1;
out vec2 v_uv2;

#ifdef HAS_VERTEX_COLOR_VEC3
    in vec3 a_color;
    out vec3 v_color;
#endif

#ifdef HAS_VERTEX_COLOR_VEC4
    in vec4 a_color;
    out vec4 v_color;
#endif

#ifdef USE_SHADOWS
    uniform u_vs_shadows {
        mat4 lightVPMatrix[MAX_SHADOWSMAP];
    };
 
    out vec4 v_lightProjPos[MAX_SHADOWSMAP];
    out float v_clipSpacePosZ;
#endif

#ifdef HAS_SKINNING
    in vec4 a_boneWeights;
    in vec4 a_boneIds;

    uniform u_vs_skinning {
        mat4 bonesMatrix[MAX_BONES];
    };
#endif

vec4 getPosition(mat4 boneTransform){
    vec3 pos = a_position;

    #ifdef HAS_SKINNING
        vec4 skinVertex = vec4(pos, 1.0);
        skinVertex = boneTransform * skinVertex;
        pos = vec3(skinVertex) / skinVertex.w;
    #endif

    return vec4(pos, 1.0);
}

#ifdef HAS_NORMALS
vec3 getNormal(mat4 boneTransform){
    vec3 normal = a_normal;

    #ifdef HAS_SKINNING
        vec4 skinNormal = vec4(normal, 1.0);
        skinNormal = boneTransform * skinNormal;
        normal = vec3(skinNormal) / skinNormal.w;
    #endif

    return normalize(normal);
}
#endif

#ifdef HAS_TANGENTS
vec3 getTangent(mat4 boneTransform){
    vec3 tangent = a_tangent.xyz;

    #ifdef HAS_SKINNING
        vec4 skinTangent = vec4(tangent, 1.0);
        skinTangent = boneTransform * skinTangent;
        tangent = vec3(skinTangent) / skinTangent.w;
    #endif

    return normalize(tangent);
}
#endif

void main() {
    mat4 boneTransform = mat4(0.0);
    #ifdef HAS_SKINNING
        //sokol send boneIds (USHORT4N) normalized, needed "expand" the normalized vertex shader
        boneTransform += bonesMatrix[int(a_boneIds[0] * 65535.0)] * a_boneWeights[0];
        boneTransform += bonesMatrix[int(a_boneIds[1] * 65535.0)] * a_boneWeights[1];
        boneTransform += bonesMatrix[int(a_boneIds[2] * 65535.0)] * a_boneWeights[2];
        boneTransform += bonesMatrix[int(a_boneIds[3] * 65535.0)] * a_boneWeights[3];
    #endif

    vec4 bonePosition = getPosition(boneTransform);
    vec4 pos = pbrParams.modelMatrix * bonePosition;
    v_position = vec3(pos.xyz) / pos.w;

    #ifdef HAS_NORMALS
    #ifdef HAS_TANGENTS
        vec3 tangent = getTangent(boneTransform);
        vec3 normalW = normalize(vec3(pbrParams.normalMatrix * vec4(getNormal(boneTransform), 0.0)));
        vec3 tangentW = normalize(vec3(pbrParams.modelMatrix * vec4(tangent, 0.0)));
        vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
        v_tbn = mat3(tangentW, bitangentW, normalW);
    #else // !HAS_TANGENTS
        v_normal = normalize(vec3(pbrParams.normalMatrix * vec4(getNormal(boneTransform), 0.0)));
    #endif
    #endif

    v_uv1 = vec2(0.0, 0.0);
    v_uv2 = vec2(0.0, 0.0);

    #ifdef HAS_UV_SET1
        v_uv1 = a_texcoord1;
        #ifdef HAS_TEXTURERECT
            v_uv1 = a_texcoord1 * spriteParams.textureRect.zw + spriteParams.textureRect.xy;
        #endif
    #endif

    #ifdef HAS_UV_SET2
        v_uv2 = a_texcoord2;
        #ifdef HAS_TEXTURERECT
            v_uv2 = a_texcoord2 * spriteParams.textureRect.zw + spriteParams.textureRect.xy;
        #endif
    #endif

    #if defined(HAS_VERTEX_COLOR_VEC3) || defined(HAS_VERTEX_COLOR_VEC4)
        v_color = a_color;
    #endif

    #ifdef USE_SHADOWS
    for (int i = 0; i < MAX_SHADOWSMAP; ++i){
        v_lightProjPos[i] = lightVPMatrix[i] * pos;
    }
    #endif

    gl_Position = pbrParams.mvpMatrix * bonePosition;

    #ifdef USE_SHADOWS
        v_clipSpacePosZ = gl_Position.z;
    #endif
}