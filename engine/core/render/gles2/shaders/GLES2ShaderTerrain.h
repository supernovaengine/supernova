//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERTERRAIN_H
#define GLES2SHADERTERRAIN_H

std::string terrainVertexDec =
        "#ifdef IS_TERRAIN\n"
        "  uniform sampler2D u_heightData;\n"

        "  uniform vec3 u_EyePos;\n"

        "  uniform float u_terrainSize;\n"
        "  uniform int u_terrainResolution;\n"

        "  uniform vec2 u_terrainNodePos;\n"
        "  uniform float u_terrainNodeSize;\n"
        "  uniform float u_terrainNodeRange;\n"
        "  uniform int u_terrainNodeResolution;\n"

        "  vec2 gridDim;\n"
        "  vec2 terrainTextureCoords;\n"

        "  vec2 morphVertex(vec2 gridPos, vec2 worldPos, float morph) {\n"
        "      vec2 fracPart = fract(gridPos * gridDim.xy * 0.5) * 2.0 / gridDim.xy;\n"
        "      return worldPos - fracPart * u_terrainNodeSize * morph;\n"
        "  }\n"

        "  float getHeight(vec3 position) {\n"
        "      return texture2DLod(u_heightData, (position.xz + (u_terrainSize/2.0)) / u_terrainSize, 0.0).r * 100.0;\n"
        "  }\n"

        "  #ifdef USE_NORMAL\n"
        "    vec3 getNormal(vec3 normal, vec3 position, float morphFactor) {\n"

        "        float delta = (morphFactor + 1.0) * u_terrainNodeSize / float(u_terrainNodeResolution);\n"

        "        vec3 dA = delta * normalize(cross(normal.yzx, normal));\n"
        "        vec3 dB = delta * normalize(cross(dA, normal));\n"
        "        vec3 p = position;\n"
        "        vec3 pA = position + dA;\n"
        "        vec3 pB = position + dB;\n"

        "        float h = getHeight(position);\n"
        "        float hA = getHeight(pA);\n"
        "        float hB = getHeight(pB);\n"

        "        p += normal * h;\n"
        "        pA += normal * hA;\n"
        "        pB += normal * hB;\n"

        "        return normalize(cross(pB - p, pA - p));\n"
        "    }\n"
        "  #endif\n"

        "#endif\n";

std::string terrainVertexImp =
        "    #ifdef IS_TERRAIN\n"
        "      localPos = u_terrainNodeSize * localPos;\n"
        "      localPos = localPos + vec3(u_terrainNodePos[0], 0.0, u_terrainNodePos[1]);\n"

        "      localPos = vec3(localPos.x, getHeight(localPos), localPos.z);\n"

        "      gridDim = vec2(u_terrainNodeResolution, u_terrainNodeResolution);\n"
        "      float morphStart = 0.0;\n"
        "      float morphEnd = 0.25;\n"

        "      float dist = distance(u_EyePos, vec3(u_mMatrix * vec4(localPos, 1.0)));\n"

        "      float nextlevel_thresh = ((u_terrainNodeRange-dist)/u_terrainNodeSize*gridDim.x/float(u_terrainResolution));\n"
        "      float morph = 1.0 - smoothstep(morphStart, morphEnd, nextlevel_thresh);\n"

        "      localPos.xz = morphVertex(a_Position.xz, localPos.xz, morph);\n"

        "      localPos = vec3(localPos.x, getHeight(localPos), localPos.z);\n"

        "      #ifdef USE_NORMAL\n"
        "        localNormal = getNormal(vec3(0.0,1.0,0.0), localPos, morph);\n"
        "      #endif\n"

        "      terrainTextureCoords = (localPos.xz + (u_terrainSize/2.0)) / u_terrainSize;\n"
        "    #endif\n";

#endif //GLES2SHADERTERRAIN_H
