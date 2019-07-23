//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERTERRAIN_H
#define GLES2SHADERTERRAIN_H

std::string terrainVertexDec =
        "#ifdef IS_TERRAIN\n"
        "  uniform vec2 u_terrainTileOffset;\n"
        "#endif\n";

std::string terrainVertexImp =
        "    #ifdef IS_TERRAIN\n"
        "      localPos += vec3(u_terrainTileOffset[0], 0.0, u_terrainTileOffset[1]);\n"
        "    #endif\n";

#endif //GLES2SHADERTERRAIN_H
