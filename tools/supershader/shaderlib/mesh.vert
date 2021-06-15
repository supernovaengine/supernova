#version 450

uniform u_vs_pbrParams {
    mat4 modelMatrix;
    mat4 normalMatrix;
    mat4 mvpMatrix;
} pbrParams;

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
    uniform u_vs_lighting {
        mat4 lightMVP[MAX_LIGHTS];
    };
    
    out vec4 lightProjPos[MAX_LIGHTS];
#endif

vec4 getPosition(){
    vec4 pos = vec4(a_position, 1.0);
    return pos;
}

#ifdef HAS_NORMALS
vec3 getNormal(){
    vec3 normal = a_normal;
    return normalize(normal);
}
#endif

#ifdef HAS_TANGENTS
vec3 getTangent(){
    vec3 tangent = a_tangent.xyz;
    return normalize(tangent);
}
#endif

void main() {
    vec4 pos = pbrParams.modelMatrix * getPosition();
    v_position = vec3(pos.xyz) / pos.w;

    #ifdef HAS_NORMALS
    #ifdef HAS_TANGENTS
        vec3 tangent = getTangent();
        vec3 normalW = normalize(vec3(pbrParams.normalMatrix * vec4(getNormal(), 0.0)));
        vec3 tangentW = normalize(vec3(pbrParams.modelMatrix * vec4(tangent, 0.0)));
        vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
        v_tbn = mat3(tangentW, bitangentW, normalW);
    #else // !HAS_TANGENTS
        v_normal = normalize(vec3(pbrParams.normalMatrix * vec4(getNormal(), 0.0)));
    #endif
    #endif

    v_uv1 = vec2(0.0, 0.0);
    v_uv2 = vec2(0.0, 0.0);

    #ifdef HAS_UV_SET1
        v_uv1 = a_texcoord1;
    #endif

    #ifdef HAS_UV_SET2
        v_uv2 = a_texcoord2;
    #endif

    #if defined(HAS_VERTEX_COLOR_VEC3) || defined(HAS_VERTEX_COLOR_VEC4)
        v_color = a_color;
    #endif

    #ifdef USE_SHADOWS
    for (int i = 0; i < MAX_LIGHTS; ++i){
        lightProjPos[i] = lightMVP[i] * getPosition();
    }
    #endif

    gl_Position = pbrParams.mvpMatrix * getPosition();
}