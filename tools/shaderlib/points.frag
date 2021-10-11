#version 450

out vec4 g_finalColor;

#ifdef HAS_VERTEX_COLOR_VEC3
    in vec3 v_color;
#endif
#ifdef HAS_VERTEX_COLOR_VEC4
    in vec4 v_color;
#endif

#ifdef HAS_TEXTURE
    uniform sampler2D u_pointsTexture;
    in vec2 v_uv1;
#endif

#include "includes/srgb.glsl"

vec4 getVertexColor(){
   vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

    #ifdef HAS_VERTEX_COLOR_VEC3
        color.rgb = v_color.rgb;
    #endif
    #ifdef HAS_VERTEX_COLOR_VEC4
        color = v_color;
    #endif

   return color;
}

vec4 getBaseColor(){
    vec4 baseColor = vec4(1.0, 1.0, 1.0, 1.0);
    #ifdef HAS_TEXTURE
        baseColor *= sRGBToLinear(texture(u_pointsTexture, v_uv1));
    #endif
    return baseColor * getVertexColor();
}

void main() {  
    vec4 color = getBaseColor();

    g_finalColor = (vec4(linearTosRGB(color.rgb), color.a));
}