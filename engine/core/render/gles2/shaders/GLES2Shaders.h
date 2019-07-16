//
// (c) 2019 Eduardo Doria.
//

#ifndef gles2shaders_h
#define gles2shaders_h

#include "GLES2ShaderMorphTarget.h"
#include "GLES2ShaderLighting.h"
#include "GLES2ShaderFog.h"
#include "GLES2ShaderTerrain.h"
#include "GLES2ShaderSkinning.h"


std::string gVertexLinesShader =
        "uniform mat4 u_mvpMatrix;\n"

        "attribute vec3 a_Position;\n"

        "void main(){\n"
        "    vec4 worldPos = u_mvpMatrix * vec4(a_Position, 1.0);\n"
        "    gl_Position = worldPos;\n"
        "}\n";

std::string gFragmentLinesShader =
        "precision highp float;\n"

        "uniform vec4 u_Color;\n"

        "void main(){\n"
        "   gl_FragColor = u_Color;\n"
        "}\n";


std::string gVertexPointsPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"

"attribute vec3 a_Position;\n"

+ lightingVertexDec +

"#ifdef HAS_TEXTURERECT\n"
"  attribute vec4 a_textureRect;\n"
"  varying vec4 v_textureRect;\n"
"#endif\n"

"attribute float a_pointSize;\n"
"attribute vec4 a_pointColor;\n"
"attribute float a_pointRotation;\n"

"varying vec4 v_pointColor;\n"
"varying float v_pointRotation;\n"

"void main(){\n"

"    vec4 localPos = vec4(a_Position, 1.0);\n"

"    vec4 worldPos = u_mvpMatrix * vec4(a_Position, 1.0);\n"

"    v_pointColor = a_pointColor;\n"
"    v_pointRotation = a_pointRotation;\n"
"    gl_PointSize = a_pointSize;\n"

"    #ifdef HAS_TEXTURERECT\n"
"      v_textureRect = a_textureRect;\n"
"    #endif\n"

+    lightingVertexImp +

"    gl_Position = worldPos;\n"
"}\n";


std::string gFragmentPointsPerPixelLightShader =
"precision highp float;\n"

"uniform sampler2D u_TextureUnit;\n"
"uniform bool uUseTexture;\n"

"varying vec4 v_pointColor;\n"
"varying float v_pointRotation;\n"

+ lightingFragmentDec
+ fogFragmentDec +

"#ifdef HAS_TEXTURERECT\n"
"  varying vec4 v_textureRect;\n"
"#endif\n"

"void main(){\n"
"   vec4 fragmentColor = v_pointColor;\n"

"   if (uUseTexture){\n"
"     vec2 resultCoord = gl_PointCoord;\n"

"     if (v_pointRotation != 0.0){\n"
"         resultCoord = vec2(cos(v_pointRotation) * (resultCoord.x - 0.5) + sin(v_pointRotation) * (resultCoord.y - 0.5) + 0.5,\n"
"                            cos(v_pointRotation) * (resultCoord.y - 0.5) - sin(v_pointRotation) * (resultCoord.x - 0.5) + 0.5);\n"
"     }\n"

"     #ifdef HAS_TEXTURERECT\n"
"       resultCoord = resultCoord * v_textureRect.zw + v_textureRect.xy;\n"
"     #endif\n"

"     fragmentColor *= texture2D(u_TextureUnit, resultCoord);\n"
"   }\n"

"   vec3 FragColor = vec3(fragmentColor);\n"

+ lightingFragmentImp
+ fogFragmentImp +

"   gl_FragColor = vec4(FragColor ,fragmentColor.a);\n"
"}\n";



std::string gVertexMeshPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"

"attribute vec3 a_Position;\n"

+ terrainVertexDec
+ morphTargetVertexDec
+ skinningVertexDec +

"#ifdef USE_TEXTURECOORDS\n"
"  attribute vec2 a_TextureCoordinates;\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n"

"#ifdef HAS_TEXTURERECT\n"
"  uniform vec4 u_textureRect;\n"
"#endif\n"

+ lightingVertexDec +

"void main(){\n"

"    vec4 localPos = vec4(a_Position, 1.0);\n"

+ terrainVertexImp
+ morphTargetVertexImp
+ skinningVertexImp +

"    vec4 worldPos = u_mvpMatrix * localPos;\n"

"    #ifdef USE_TEXTURECOORDS\n"
"      #ifdef USE_TEXTURECUBE\n"
"        v_TextureCoordinates = a_Position;\n"
"      #else\n"
"        #ifdef HAS_TEXTURERECT\n"
"          vec2 resultCoords = a_TextureCoordinates * u_textureRect.zw + u_textureRect.xy;\n"
"        #else\n"
"          vec2 resultCoords = a_TextureCoordinates;\n"
"        #endif\n"
"        v_TextureCoordinates = vec3(resultCoords,0.0);\n"
"      #endif\n"
"    #endif\n"

"    #ifdef IS_SKY\n"
"      worldPos.z = worldPos.w;\n"
"    #endif\n"

+ lightingVertexImp +

"    gl_Position = worldPos;\n"

"}\n";

std::string gFragmentMeshPerPixelLightShader =
"precision highp float;\n"

"#ifdef USE_TEXTURECUBE\n"
"  uniform samplerCube u_TextureUnit;\n"
"#else\n"
"  uniform sampler2D u_TextureUnit;\n"
"#endif\n"

"uniform vec4 u_Color;\n"

"uniform bool uUseTexture;\n"

+ lightingFragmentDec
+ fogFragmentDec +

"#ifdef USE_TEXTURECOORDS\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n"

"void main(){\n"

    //Texture or color
"   vec4 fragmentColor = u_Color;\n"

"   if (uUseTexture){\n"
"     #ifdef USE_TEXTURECOORDS\n"
"       #ifdef USE_TEXTURECUBE\n"
"         fragmentColor *= textureCube(u_TextureUnit, v_TextureCoordinates);\n"
"       #else\n"
"         #ifdef IS_TEXT\n"
"           fragmentColor *= vec4(1.0, 1.0, 1.0, texture2D(u_TextureUnit, v_TextureCoordinates.xy).a);\n"
"         #else\n"
"           fragmentColor *= texture2D(u_TextureUnit, v_TextureCoordinates.xy);\n"
"         #endif\n"
"       #endif\n"
"     #endif\n"
"   }\n"

"   vec3 FragColor = vec3(fragmentColor);\n"

+ lightingFragmentImp
+ fogFragmentImp +

"   gl_FragColor = vec4(FragColor ,fragmentColor.a);\n"
"}\n";

std::string gVertexDepthShader =
"uniform mat4 u_mvpMatrix;\n"
"uniform mat4 u_mMatrix;\n"
"attribute vec3 a_Position;\n"
"varying vec3 v_position;\n"
+ terrainVertexDec
+ morphTargetVertexDec
+ skinningVertexDec +
"void main(){\n"
"    vec4 localPos = vec4(a_Position, 1.0);\n"
+ terrainVertexImp
+ morphTargetVertexImp
+ skinningVertexImp +
"    v_position = vec3(u_mMatrix * localPos);\n"
"    gl_Position = u_mvpMatrix * localPos;\n"
"}\n";

std::string gFragmentDepthShader =
"precision highp float;\n"

"varying vec3 v_position;\n"
"uniform vec3 u_shadowLightPos;\n"
"uniform vec2 u_shadowCameraNearFar;\n"
"uniform bool u_isPointShadow;\n"

"vec4 packDepth(const in float depth) {\n"
"    const vec4 bitShift = vec4(255.0 * 255.0 * 255.0, 255.0 * 255.0, 255.0, 1.0);\n"
"    const vec4 bitMask = vec4(0.0, 1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0);\n"
"    vec4 res = fract(depth * bitShift);\n"
"    res -= res.xxyz * bitMask;\n"
"    return res;\n"
"}\n"

"void main(){\n"
"    if (u_isPointShadow){\n"
"        float lightDistance = length(v_position - u_shadowLightPos);\n"
"        lightDistance = (lightDistance - u_shadowCameraNearFar.x) / (u_shadowCameraNearFar.y - u_shadowCameraNearFar.x);\n"
"        gl_FragColor = packDepth(lightDistance);\n"
"    }else{\n"
//"        gl_FragColor = packDepth(gl_FragCoord.z);\n"
"    }\n"
"}\n";

#endif /* gles2_shaders_h */
