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
#include "GLES2ShaderMeshTexture.h"
#include "GLES2ShaderPointTexture.h"


std::string gVertexLinesShader =
        "uniform mat4 u_mvpMatrix;\n"

        "attribute vec3 a_Position;\n"

        "void main(){\n"
        "    vec4 mvpPos = u_mvpMatrix * vec4(a_Position, 1.0);\n"
        "    gl_Position = mvpPos;\n"
        "}\n";

std::string gFragmentLinesShader =
        "precision highp float;\n"

        "uniform vec4 u_Color;\n"

        "void main(){\n"
        "   gl_FragColor = u_Color;\n"
        "}\n";

std::string gVertexPointsPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"
"uniform mat4 u_mMatrix;\n"

"attribute vec3 a_Position;\n"
"#ifdef USE_NORMAL\n"
"  attribute vec3 a_Normal;\n"
"#endif\n"

+ lightingVertexDec
+ texturePointVertexDec +

"attribute float a_pointSize;\n"
"attribute vec4 a_pointColor;\n"
"attribute float a_pointRotation;\n"

"varying vec4 v_pointColor;\n"
"varying float v_pointRotation;\n"

"void main(){\n"

"    vec3 localPos = a_Position;\n"
"    #ifdef USE_NORMAL\n"
"      vec3 localNormal = a_Normal;\n"
"    #endif\n"

"    vec4 mvpPos = u_mvpMatrix * vec4(localPos, 1.0);\n"

"    v_pointColor = a_pointColor;\n"
"    v_pointRotation = a_pointRotation;\n"
"    gl_PointSize = a_pointSize;\n"

+ texturePointVertexImp
+ lightingVertexImp +

"    gl_Position = mvpPos;\n"
"}\n";

std::string gFragmentPointsPerPixelLightShader =
"precision highp float;\n"
"varying vec4 v_pointColor;\n"
"varying float v_pointRotation;\n"
+ texturePointFragmentDec
+ lightingFragmentDec
+ fogFragmentDec +
"void main(){\n"
"   vec4 fragColor = v_pointColor;\n"
+ texturePointFragmentImp
+ lightingFragmentImp
+ fogFragmentImp +
"   gl_FragColor = fragColor;\n"
"}\n";

std::string gVertexMeshPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"
"uniform mat4 u_mMatrix;\n"

"attribute vec3 a_Position;\n"
"#ifdef USE_NORMAL\n"
"  attribute vec3 a_Normal;\n"
"#endif\n"

+ terrainVertexDec
+ morphTargetVertexDec
+ skinningVertexDec
+ textureMeshVertexDec
+ lightingVertexDec +

"void main(){\n"
"    vec3 localPos = a_Position;\n"
"    #ifdef USE_NORMAL\n"
"      vec3 localNormal = a_Normal;\n"
"    #endif\n"

+ terrainVertexImp
+ morphTargetVertexImp
+ skinningVertexImp
+ textureMeshVertexImp
+ lightingVertexImp +

"    vec4 mvpPos = u_mvpMatrix * vec4(localPos, 1.0);\n"

"    #ifdef IS_SKY\n"
"      mvpPos.z = mvpPos.w;\n"
"    #endif\n"

"    gl_Position = mvpPos;\n"

"}\n";

std::string gFragmentMeshPerPixelLightShader =
"precision highp float;\n"
"uniform vec4 u_Color;\n"
+ lightingFragmentDec
+ fogFragmentDec
+ textureMeshFragmentDec +
"void main(){\n"
"   vec4 fragColor = u_Color;\n"
+ textureMeshFragmentImp
+ lightingFragmentImp
+ fogFragmentImp +
"   gl_FragColor = fragColor;\n"
"}\n";

std::string gVertexDepthShader =
"uniform mat4 u_mvpMatrix;\n"
"uniform mat4 u_mMatrix;\n"
"attribute vec3 a_Position;\n"
"varying vec3 v_worldPos;\n"
+ terrainVertexDec
+ morphTargetVertexDec
+ skinningVertexDec +
"void main(){\n"
"    vec3 localPos = a_Position;\n"
+ terrainVertexImp
+ morphTargetVertexImp
+ skinningVertexImp +
"    v_worldPos = vec3(u_mMatrix * vec4(localPos, 1.0));\n"
"    gl_Position = u_mvpMatrix * vec4(localPos, 1.0);\n"
"}\n";

std::string gFragmentDepthShader =
"precision highp float;\n"

"varying vec3 v_worldPos;\n"
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
"        float lightDistance = length(v_worldPos - u_shadowLightPos);\n"
"        lightDistance = (lightDistance - u_shadowCameraNearFar.x) / (u_shadowCameraNearFar.y - u_shadowCameraNearFar.x);\n"
"        gl_FragColor = packDepth(lightDistance);\n"
"    }else{\n"
//"        gl_FragColor = packDepth(gl_FragCoord.z);\n"
"    }\n"
"}\n";

#endif /* gles2_shaders_h */
