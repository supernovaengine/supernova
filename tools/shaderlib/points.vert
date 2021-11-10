#version 450

uniform u_vs_pointsParams {
    mat4 mvpMatrix;
} pointsParams;

in vec3 a_position;

in float a_pointsize;

in float a_pointrotation;
out float v_pointrotation;

#ifdef HAS_VERTEX_COLOR_VEC3
    in vec3 a_color;
    out vec3 v_color;
#endif

#ifdef HAS_VERTEX_COLOR_VEC4
    in vec4 a_color;
    out vec4 v_color;
#endif

#ifdef HAS_TEXTURERECT
    in vec4 a_texturerect;
    out vec4 v_texturerect;
#endif


void main() {

    v_pointrotation = a_pointrotation;

    #if defined(HAS_VERTEX_COLOR_VEC3) || defined(HAS_VERTEX_COLOR_VEC4)
        v_color = a_color;
    #endif

    #ifdef HAS_TEXTURERECT
        v_texturerect = a_texturerect;
    #endif

    gl_Position = pointsParams.mvpMatrix * vec4(a_position, 1.0);
    gl_PointSize = a_pointsize / gl_Position.w;
}