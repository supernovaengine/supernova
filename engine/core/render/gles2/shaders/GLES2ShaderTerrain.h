//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERTERRAIN_H
#define GLES2SHADERTERRAIN_H

std::string terrainVertexDec =
        "#ifdef IS_TERRAIN\n"
        "  uniform sampler2D u_heightData;\n"

        "  uniform vec2 u_terrainGlobalOffset;\n"
        "  uniform vec2 u_terrainNodePos;\n"
        "  uniform float u_terrainNodeSize;\n"

        "  float getHeight(vec3 p) {\n"
              // Assume a 1024x1024 world
        "      float lod = 0.0;//log2(uScale) - 6.0;\n"
        "      vec2 st = p.xz / 2048.0;\n"

              // Sample multiple times to get more detail out of map
        "     float h = 1024.0 * texture2DLod(u_heightData, st, lod).r;\n"
        //"      h += 64.0 * texture2DLod(u_heightData, 16.0 * st, lod).r;\n"
        //"      h += 4.0 * texture2DLod(u_heightData, 256.0 * st, lod).r;\n"

              // Square the height, leads to more rocky looking terrain
        "      return h * h / 2000.0;\n"
        //"      return h / 10.0;\n"
        "  }\n"

        "#endif\n";

std::string terrainVertexImp =
        "    #ifdef IS_TERRAIN\n"
        "      localPos = u_terrainNodeSize * localPos;\n"
        "      localPos = localPos + vec3(u_terrainNodePos[0], 0.0, u_terrainNodePos[1]);\n"
        //"      localPos = localPos + vec3(u_terrainGlobalOffset[0], 0.0, u_terrainGlobalOffset[1]);\n"
        "      localPos = vec3(localPos.x, getHeight(localPos), localPos.z);\n"
        "    #endif\n";

#endif //GLES2SHADERTERRAIN_H
