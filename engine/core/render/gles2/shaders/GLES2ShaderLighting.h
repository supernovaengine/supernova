//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERLIGHTING_H
#define GLES2SHADERLIGHTING_H

std::string lightingVertexDec =
        "#ifdef USE_LIGHTING\n"

        "  uniform mat4 u_nMatrix;\n"

        "  varying vec3 v_worldPos;\n"
        "  varying vec3 v_worldNormal;\n"

        "  #ifdef HAS_SHADOWS2D\n"
        "    uniform int u_NumShadows2D;\n"
        "    uniform mat4 u_ShadowVP[MAXSHADOWS2D];\n"
        "    varying vec4 v_ShadowCoordinates[MAXSHADOWS2D];\n"
        "  #endif\n"

        "#endif\n";

std::string lightingVertexImp =
        "#ifdef USE_LIGHTING\n"

        "  v_worldPos = vec3(u_mMatrix * vec4(localPos, 1.0));\n"
        "  v_worldNormal = normalize(vec3(u_nMatrix * vec4(localNormal, 1.0)));\n"

        "  #ifdef HAS_SHADOWS2D\n"
        "    for(int i=0; i<MAXSHADOWS2D; ++i){\n"
        "        if (i < u_NumShadows2D){\n"
        "            v_ShadowCoordinates[i] = u_ShadowVP[i] * vec4(v_worldPos, 1.0);\n"
        "        }else{\n"
        "            v_ShadowCoordinates[i] = vec4(0.0);\n"
        "        }\n"
        "    }\n"
        "  #endif\n"

        "#endif\n";

std::string lightingFragmentDec =
        "#ifdef USE_LIGHTING\n"

        "  uniform vec3 u_EyePos;\n"

        "  uniform vec3 u_AmbientLight;\n"

        "  uniform int u_NumPointLight;\n"
        "  uniform vec3 u_PointLightPos[MAXLIGHTS];\n"
        "  uniform vec3 u_PointLightColor[MAXLIGHTS];\n"
        "  uniform float u_PointLightPower[MAXLIGHTS];\n"
        "  uniform int u_PointLightShadowIdx[MAXLIGHTS];\n"

        "  uniform int u_NumSpotLight;\n"
        "  uniform vec3 u_SpotLightPos[MAXLIGHTS];\n"
        "  uniform vec3 u_SpotLightColor[MAXLIGHTS];\n"
        "  uniform float u_SpotLightPower[MAXLIGHTS];\n"
        "  uniform vec3 u_SpotLightTarget[MAXLIGHTS];\n"
        "  uniform float u_SpotLightCutOff[MAXLIGHTS];\n"
        "  uniform float u_SpotLightOuterCutOff[MAXLIGHTS];\n"
        "  uniform int u_SpotLightShadowIdx[MAXLIGHTS];\n"

        "  uniform int u_NumDirectionalLight;\n"
        "  uniform vec3 u_DirectionalLightDir[MAXLIGHTS];\n"
        "  uniform float u_DirectionalLightPower[MAXLIGHTS];\n"
        "  uniform vec3 u_DirectionalLightColor[MAXLIGHTS];\n"
        "  uniform int u_DirectionalLightShadowIdx[MAXLIGHTS];\n"

        "  varying vec3 v_worldPos;\n"
        "  varying vec3 v_worldNormal;\n"

        "  #ifdef HAS_SHADOWS2D\n"
        "    uniform sampler2D u_shadowsMap2D[MAXSHADOWS2D];\n"
        "    uniform float u_shadowBias2D[MAXSHADOWS2D];\n"
        "    uniform vec2 u_shadowCameraNearFar2D[MAXSHADOWS2D];\n"
        "    uniform int u_shadowNumCascades2D[MAXSHADOWS2D];\n"
        "    varying vec4 v_ShadowCoordinates[MAXSHADOWS2D];\n"
        "  #endif\n"
        "  #ifdef HAS_SHADOWSCUBE\n"
        "    uniform samplerCube u_shadowsMapCube[MAXSHADOWSCUBE];\n"
        "    uniform float u_shadowBiasCube[MAXSHADOWSCUBE];\n"
        "    uniform vec2 u_shadowCameraNearFarCube[MAXSHADOWSCUBE];\n"
        "  #endif\n"

        "  float unpackDepth(in vec4 color) {\n"
        "      const vec4 bitShift = vec4(1.0 / (255.0 * 255.0 * 255.0), 1.0 / (255.0 * 255.0), 1.0 / 255.0, 1.0);\n"
        "     return dot(color, bitShift);\n"
        "  }\n"

        "  #ifdef HAS_SHADOWS2D\n"
        "    bool checkShadow(vec4 shadowCoordinates, sampler2D shadowMap, float shadowBias, vec2 shadowCameraNearFar, float cosTheta, float clipSpacePosZ) {\n"
        "        if ((clipSpacePosZ >= shadowCameraNearFar.x) && (clipSpacePosZ <= shadowCameraNearFar.y)){\n"
        "            vec3 shadowCoord = (shadowCoordinates.xyz/shadowCoordinates.w)/2.0 + 0.5;\n"
        "            vec4 rgbaDepth = texture2D(shadowMap, shadowCoord.xy);\n"
        //"          float depth = unpackDepth(rgbaDepth);\n"
        "            float depth = rgbaDepth.r;\n"
        "            float bias = shadowBias*tan(acos(cosTheta));\n"
        "            bias = clamp(bias, 0.0000001, 0.01);\n"
        "            if (shadowCoord.z > depth + bias){\n"
        "                return true;\n"
        "            }\n"
        "        }\n"
        "        return false;\n"
        "    }\n"
        "  #endif\n"

        "  #ifdef HAS_SHADOWSCUBE\n"
        "    bool checkShadowCube(vec3 lightPos, samplerCube shadowMap, float shadowBias, vec2 shadowCameraNearFar) {\n"
        "        lightPos = vec3(lightPos.x, lightPos.y, lightPos.z);\n"
        "        vec3 fragToLight = v_worldPos - lightPos;\n"
        "        float lenDepthMap = unpackDepth(textureCube(shadowMap, fragToLight));\n"
        "        float lenToLight = (length(fragToLight) - shadowCameraNearFar.x) / (shadowCameraNearFar.y - shadowCameraNearFar.x);\n"
        "        float bias = shadowBias;\n"
        "        if ((lenToLight < 1.0 - bias) && (lenToLight > lenDepthMap + bias)){\n"
        "            return true;\n"
        "        }\n"
        "        return false;\n"
        "    }\n"
        "  #endif\n"

        "#endif\n";

std::string lightingFragmentImp =

        "   #ifdef USE_LIGHTING\n"

        "     vec3 PointLightFalloff = vec3(0.0,1.0,0.0);\n"
        "     vec3 SpotLightFalloff = vec3(0.0,1.0,0.0);\n"

        "     vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);\n"
        "     float MaterialShininess = 80.0;\n"

        "     vec3 MaterialDiffuseColor = vec3(fragColor);\n"
        "     vec3 MaterialAmbientColor = u_AmbientLight * MaterialDiffuseColor;\n"

        "     vec3 lightFragColor = MaterialAmbientColor;\n"

        "     vec3 EyeDirection = normalize( u_EyePos - v_worldPos );\n"

        "     int numLights = u_NumPointLight + u_NumSpotLight + u_NumDirectionalLight;\n"

        "     float clipSpacePosZ = (gl_FragCoord.z / gl_FragCoord.w);\n" //The same of mvpPos.z

        "     for(int i = 0; i < MAXLIGHTS; ++i){\n"
        //PointLight
        "         if (i < u_NumPointLight){\n"

        "             bool inShadow = false;\n"
        "             #ifdef HAS_SHADOWSCUBE\n"
        "               #pragma unroll_loop\n"
        "               for(int j = 0; j < MAXSHADOWSCUBE; j++){\n"
        "                   if (u_PointLightShadowIdx[i] == (j))\n"
        "                       inShadow = checkShadowCube(u_PointLightPos[i], u_shadowsMapCube[j], u_shadowBiasCube[j], u_shadowCameraNearFarCube[j]);\n"
        "               }\n"
        "             #endif\n"

        "             if (!inShadow) {\n"
        "                 float PointLightDistance = length(u_PointLightPos[i] - v_worldPos);\n"

        "                 vec3 PointLightDirection = normalize( u_PointLightPos[i] - v_worldPos );\n"
        "                 float PointLightcosTheta = clamp( dot( v_worldNormal,PointLightDirection ), 0.0,1.0 );\n"

        "                 float PointLightcosAlpha = 0.0;\n"
        "                 if (PointLightcosTheta > 0.0){\n"
        "                    float PointLightcosAlpha = clamp( dot( EyeDirection, reflect(-PointLightDirection,v_worldNormal) ), 0.0,1.0 );\n"
        "                 }\n"

        "                 float Attenuation = PointLightFalloff.x + (PointLightFalloff.y*PointLightDistance) + (PointLightFalloff.z*PointLightDistance*PointLightDistance);\n"

        "                 lightFragColor = lightFragColor +\n"
        "                   ( MaterialDiffuseColor * PointLightcosTheta \n"
        "                   + MaterialSpecularColor * pow(PointLightcosAlpha, MaterialShininess) ) \n"
        "                   * u_PointLightColor[i] * u_PointLightPower[i] / Attenuation;\n"
        "             }\n"

        "         }\n"

        //SpotLight
        "         if (i < u_NumSpotLight){\n"

        "             vec3 SpotLightDirection = normalize( u_SpotLightPos[i] - v_worldPos );\n"
        "             float SpotLightcosTheta = clamp( dot( v_worldNormal,SpotLightDirection ), 0.0,1.0 );\n"

        "             bool inShadow = false;\n"
        "             #ifdef HAS_SHADOWS2D\n"
        "               #pragma unroll_loop\n"
        "               for(int j = 0; j < MAXSHADOWS2D; j++){\n"
        "                   if (u_SpotLightShadowIdx[i] == (j))\n"
        "                       inShadow = checkShadow(v_ShadowCoordinates[j], u_shadowsMap2D[j], u_shadowBias2D[j], u_shadowCameraNearFar2D[j], SpotLightcosTheta, clipSpacePosZ);\n"
        "               }\n"
        "             #endif\n"

        "             if (!inShadow) {\n"
        "                 float SpotLightDistance = length(u_SpotLightPos[i] - v_worldPos);\n"

        "                 float SpotLightcosAlpha = 0.0;\n"
        "                 if (SpotLightcosTheta > 0.0){\n"
        "                     SpotLightcosAlpha = clamp( dot( EyeDirection, reflect(-SpotLightDirection,v_worldNormal) ), 0.0,1.0 );\n"
        "                 }\n"

        "                 vec3 SpotLightTargetNorm = normalize( u_SpotLightTarget[i] - u_SpotLightPos[i] );\n"
        "                 float SpotLightepsilon = (u_SpotLightCutOff[i] - u_SpotLightOuterCutOff[i]);\n"
        "                 float SpotLightintensity = clamp((dot( SpotLightTargetNorm, -SpotLightDirection ) - u_SpotLightOuterCutOff[i]) / SpotLightepsilon, 0.0, 1.0);\n"

        //"               if ( dot( SpotLightTargetNorm, -SpotLightDirection ) > u_SpotLightCutOff[i] ) {\n"
        "                 float Attenuation = SpotLightFalloff.x + (SpotLightFalloff.y*SpotLightDistance) + (SpotLightFalloff.z*SpotLightDistance*SpotLightDistance);\n"

        "                 lightFragColor = lightFragColor +\n"
        "                   ( MaterialDiffuseColor * SpotLightcosTheta \n"
        "                   + MaterialSpecularColor * pow(SpotLightcosAlpha, MaterialShininess) ) \n"
        "                   * u_SpotLightColor[i] * SpotLightintensity * u_SpotLightPower[i] / Attenuation;\n"
        "             }\n"

        "         }\n"

        //DirectionalLight
        "         if (i < u_NumDirectionalLight){\n"

        "             vec3 DirectionalLightDirection = normalize( -u_DirectionalLightDir[i] );\n"
        "             float DirectionalLightcosTheta = clamp( dot( v_worldNormal,DirectionalLightDirection ), 0.0,1.0 );\n"

        "             int nShadows = 0;\n"
        "             #ifdef HAS_SHADOWS2D\n"
        "               #pragma unroll_loop\n"
        "               for(int j = 0; j < MAXSHADOWS2D; j++){\n"
        "                   if ((u_DirectionalLightShadowIdx[i] <= (j)) && ((u_DirectionalLightShadowIdx[i] + u_shadowNumCascades2D[j]) > (j)))\n"
        "                       if (checkShadow(v_ShadowCoordinates[j], u_shadowsMap2D[j], u_shadowBias2D[j], u_shadowCameraNearFar2D[j], DirectionalLightcosTheta, clipSpacePosZ))\n"
        "                           nShadows += 1;\n"
        "               }\n"
        "             #endif\n"

        "             if (nShadows == 0) {\n"
        "                 float DirectionalLightcosAlpha = 0.0;\n"
        "                 if (DirectionalLightcosTheta > 0.0){\n"
        "                     DirectionalLightcosAlpha = clamp( dot( EyeDirection, reflect(-DirectionalLightDirection,v_worldNormal) ), 0.0,1.0 );\n"
        "                 }\n"

        "                 lightFragColor = lightFragColor +\n"
        "                   ( MaterialDiffuseColor * DirectionalLightcosTheta \n"
        "                   + MaterialSpecularColor * pow(DirectionalLightcosAlpha, MaterialShininess) ) \n"
        "                   * u_DirectionalLightColor[i] * u_DirectionalLightPower[i];\n"
        "             }\n"

        "         }\n"

        "     }\n"

        "     fragColor = vec4(lightFragColor, fragColor.a);\n"

        "   #endif\n";


#endif //GLES2SHADERLIGHTING_H
