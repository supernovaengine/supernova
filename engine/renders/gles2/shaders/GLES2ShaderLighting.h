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

        "  #ifdef USE_SHADOWS2D\n"
        "    uniform mat4 u_ShadowVP[NUMSHADOWS2D];\n"
        "    varying vec4 v_ShadowCoordinates[NUMSHADOWS2D];\n"
        "  #endif\n"

        "#endif\n";

std::string lightingVertexImp =
        "#ifdef USE_LIGHTING\n"

        "  v_worldPos = vec3(u_mMatrix * vec4(localPos, 1.0));\n"
        "  v_worldNormal = normalize(vec3(u_nMatrix * vec4(localNormal, 1.0)));\n"

        "  #ifdef USE_SHADOWS2D\n"
        "    #pragma unroll_loop\n"
        "    for(int i = 0; i < NUMSHADOWS2D; ++i){\n"
        "        v_ShadowCoordinates[i] = u_ShadowVP[i] * vec4(v_worldPos, 1.0);\n"
        "    }\n"
        "  #endif\n"

        "#endif\n";

std::string lightingFragmentDec =
        "#ifdef USE_LIGHTING\n"

        "  uniform vec3 u_AmbientLight;\n"

        "  #ifdef USE_POINTLIGHT\n"
        "    uniform vec3 u_PointLightPos[NUMPOINTLIGHTS];\n"
        "    uniform vec3 u_PointLightColor[NUMPOINTLIGHTS];\n"
        "    uniform float u_PointLightPower[NUMPOINTLIGHTS];\n"
        "    uniform int u_PointLightShadowIdx[NUMPOINTLIGHTS];\n"
        "  #endif\n"

        "  #ifdef USE_SPOTLIGHT\n"
        "    uniform vec3 u_SpotLightPos[NUMSPOTLIGHTS];\n"
        "    uniform vec3 u_SpotLightColor[NUMSPOTLIGHTS];\n"
        "    uniform float u_SpotLightPower[NUMSPOTLIGHTS];\n"
        "    uniform vec3 u_SpotLightTarget[NUMSPOTLIGHTS];\n"
        "    uniform float u_SpotLightCutOff[NUMSPOTLIGHTS];\n"
        "    uniform float u_SpotLightOuterCutOff[NUMSPOTLIGHTS];\n"
        "    uniform int u_SpotLightShadowIdx[NUMSPOTLIGHTS];\n"
        "  #endif\n"

        "  #ifdef USE_DIRLIGHT\n"
        "    uniform vec3 u_DirectionalLightDir[NUMDIRLIGHTS];\n"
        "    uniform float u_DirectionalLightPower[NUMDIRLIGHTS];\n"
        "    uniform vec3 u_DirectionalLightColor[NUMDIRLIGHTS];\n"
        "    uniform int u_DirectionalLightShadowIdx[NUMDIRLIGHTS];\n"
        "  #endif\n"

        "  varying vec3 v_worldPos;\n"
        "  varying vec3 v_worldNormal;\n"

        "  #ifdef USE_SHADOWS2D\n"
        "    uniform sampler2D u_shadowsMap2D[NUMSHADOWS2D];\n"
        "    uniform float u_shadowBias2D[NUMSHADOWS2D];\n"
        "    uniform vec2 u_shadowCameraNearFar2D[NUMSHADOWS2D];\n"
        "    uniform int u_shadowNumCascades2D[NUMSHADOWS2D];\n"
        "    varying vec4 v_ShadowCoordinates[NUMSHADOWS2D];\n"
        "  #endif\n"
        "  #ifdef USE_SHADOWSCUBE\n"
        "    uniform samplerCube u_shadowsMapCube[NUMSHADOWSCUBE];\n"
        "    uniform float u_shadowBiasCube[NUMSHADOWSCUBE];\n"
        "    uniform vec2 u_shadowCameraNearFarCube[NUMSHADOWSCUBE];\n"
        "  #endif\n"

        "  float unpackDepth(in vec4 color) {\n"
        "      const vec4 bitShift = vec4(1.0 / (255.0 * 255.0 * 255.0), 1.0 / (255.0 * 255.0), 1.0 / 255.0, 1.0);\n"
        "     return dot(color, bitShift);\n"
        "  }\n"

        "  #ifdef USE_SHADOWS2D\n"
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

        "  #ifdef USE_SHADOWSCUBE\n"
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

        "#ifdef USE_LIGHTING\n"

        "  vec3 PointLightFalloff = vec3(0.0,1.0,0.0);\n"
        "  vec3 SpotLightFalloff = vec3(0.0,1.0,0.0);\n"

        "  vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);\n"
        "  float MaterialShininess = 80.0;\n"

        "  vec3 MaterialDiffuseColor = vec3(fragColor);\n"
        "  vec3 MaterialAmbientColor = u_AmbientLight * MaterialDiffuseColor;\n"

        "  vec3 lightFragColor = MaterialAmbientColor;\n"

        "  vec3 EyeDirection = normalize( u_EyePos - v_worldPos );\n"

        "  float clipSpacePosZ = (gl_FragCoord.z / gl_FragCoord.w);\n" //The same of mvpPos.z

        //PointLight
        "  #ifdef USE_POINTLIGHT\n"
        "    for(int i = 0; i < NUMPOINTLIGHTS; ++i){\n"

        "        bool inShadow = false;\n"
        "        #ifdef USE_SHADOWSCUBE\n"
        "          #pragma unroll_loop\n"
        "          for(int j = 0; j < NUMSHADOWSCUBE; j++){\n"
        "              if (u_PointLightShadowIdx[i] == (j))\n"
        "                  inShadow = checkShadowCube(u_PointLightPos[i], u_shadowsMapCube[j], u_shadowBiasCube[j], u_shadowCameraNearFarCube[j]);\n"
        "          }\n"
        "        #endif\n"

        "        if (!inShadow) {\n"
        "            float PointLightDistance = length(u_PointLightPos[i] - v_worldPos);\n"

        "            vec3 PointLightDirection = normalize( u_PointLightPos[i] - v_worldPos );\n"
        "            float PointLightcosTheta = clamp( dot( v_worldNormal,PointLightDirection ), 0.0,1.0 );\n"

        "            float PointLightcosAlpha = 0.0;\n"
        "            if (PointLightcosTheta > 0.0){\n"
        "               float PointLightcosAlpha = clamp( dot( EyeDirection, reflect(-PointLightDirection,v_worldNormal) ), 0.0,1.0 );\n"
        "            }\n"

        "            float Attenuation = PointLightFalloff.x + (PointLightFalloff.y*PointLightDistance) + (PointLightFalloff.z*PointLightDistance*PointLightDistance);\n"

        "            lightFragColor = lightFragColor +\n"
        "              ( MaterialDiffuseColor * PointLightcosTheta \n"
        "              + MaterialSpecularColor * pow(PointLightcosAlpha, MaterialShininess) ) \n"
        "              * u_PointLightColor[i] * u_PointLightPower[i] / Attenuation;\n"
        "        }\n"
        "    }\n"
        "  #endif\n"

        //SpotLight
        "  #ifdef USE_SPOTLIGHT\n"
        "    for(int i = 0; i < NUMSPOTLIGHTS; ++i){\n"

        "        vec3 SpotLightDirection = normalize( u_SpotLightPos[i] - v_worldPos );\n"
        "        float SpotLightcosTheta = clamp( dot( v_worldNormal,SpotLightDirection ), 0.0,1.0 );\n"

        "        bool inShadow = false;\n"
        "        #ifdef USE_SHADOWS2D\n"
        "          #pragma unroll_loop\n"
        "          for(int j = 0; j < NUMSHADOWS2D; j++){\n"
        "              if (u_SpotLightShadowIdx[i] == (j))\n"
        "                  inShadow = checkShadow(v_ShadowCoordinates[j], u_shadowsMap2D[j], u_shadowBias2D[j], u_shadowCameraNearFar2D[j], SpotLightcosTheta, clipSpacePosZ);\n"
        "          }\n"
        "        #endif\n"

        "        if (!inShadow) {\n"
        "            float SpotLightDistance = length(u_SpotLightPos[i] - v_worldPos);\n"

        "            float SpotLightcosAlpha = 0.0;\n"
        "            if (SpotLightcosTheta > 0.0){\n"
        "                SpotLightcosAlpha = clamp( dot( EyeDirection, reflect(-SpotLightDirection,v_worldNormal) ), 0.0,1.0 );\n"
        "            }\n"

        "            vec3 SpotLightTargetNorm = normalize( u_SpotLightTarget[i] - u_SpotLightPos[i] );\n"
        "            float SpotLightepsilon = (u_SpotLightCutOff[i] - u_SpotLightOuterCutOff[i]);\n"
        "            float SpotLightintensity = clamp((dot( SpotLightTargetNorm, -SpotLightDirection ) - u_SpotLightOuterCutOff[i]) / SpotLightepsilon, 0.0, 1.0);\n"

        //"          if ( dot( SpotLightTargetNorm, -SpotLightDirection ) > u_SpotLightCutOff[i] ) {\n"
        "            float Attenuation = SpotLightFalloff.x + (SpotLightFalloff.y*SpotLightDistance) + (SpotLightFalloff.z*SpotLightDistance*SpotLightDistance);\n"

        "            lightFragColor = lightFragColor +\n"
        "              ( MaterialDiffuseColor * SpotLightcosTheta \n"
        "              + MaterialSpecularColor * pow(SpotLightcosAlpha, MaterialShininess) ) \n"
        "              * u_SpotLightColor[i] * SpotLightintensity * u_SpotLightPower[i] / Attenuation;\n"
        "        }\n"
        "    }\n"
        "  #endif\n"

        //DirectionalLight
        "  #ifdef USE_DIRLIGHT\n"
        "    for(int i = 0; i < NUMDIRLIGHTS; ++i){\n"

        "        vec3 DirectionalLightDirection = normalize( -u_DirectionalLightDir[i] );\n"
        "        float DirectionalLightcosTheta = clamp( dot( v_worldNormal,DirectionalLightDirection ), 0.0,1.0 );\n"

        "        int nShadows = 0;\n"
        "        #ifdef USE_SHADOWS2D\n"
        "          #pragma unroll_loop\n"
        "          for(int j = 0; j < NUMSHADOWS2D; j++){\n"
        "              if ((u_DirectionalLightShadowIdx[i] <= (j)) && ((u_DirectionalLightShadowIdx[i] + u_shadowNumCascades2D[j]) > (j)))\n"
        "                  if (checkShadow(v_ShadowCoordinates[j], u_shadowsMap2D[j], u_shadowBias2D[j], u_shadowCameraNearFar2D[j], DirectionalLightcosTheta, clipSpacePosZ))\n"
        "                      nShadows += 1;\n"
        "          }\n"
        "        #endif\n"

        "        if (nShadows == 0) {\n"
        "            float DirectionalLightcosAlpha = 0.0;\n"
        "            if (DirectionalLightcosTheta > 0.0){\n"
        "                DirectionalLightcosAlpha = clamp( dot( EyeDirection, reflect(-DirectionalLightDirection,v_worldNormal) ), 0.0,1.0 );\n"
        "            }\n"

        "            lightFragColor = lightFragColor +\n"
        "              ( MaterialDiffuseColor * DirectionalLightcosTheta \n"
        "              + MaterialSpecularColor * pow(DirectionalLightcosAlpha, MaterialShininess) ) \n"
        "              * u_DirectionalLightColor[i] * u_DirectionalLightPower[i];\n"
        "        }\n"
        "    }\n"
        "  #endif\n"

        "  fragColor = vec4(lightFragColor, fragColor.a);\n"

        "#endif\n";


#endif //GLES2SHADERLIGHTING_H
