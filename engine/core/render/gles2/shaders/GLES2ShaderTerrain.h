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
        "      localPos += vec4(u_terrainTileOffset.x, 0.0, u_terrainTileOffset.y, 0.0);\n"
        "    #endif\n";

#endif //GLES2SHADERTERRAIN_H
