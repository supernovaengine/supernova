//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERMORPHTARGET_H
#define GLES2SHADERMORPHTARGET_H

std::string morphTargetVertexDec =
        "#ifdef USE_MORPHTARGET\n"
        "  attribute vec3 a_morphTarget0;\n"
        "  attribute vec3 a_morphTarget1;\n"
        "  attribute vec3 a_morphTarget2;\n"
        "  attribute vec3 a_morphTarget3;\n"
        "  attribute vec3 a_morphTarget4;\n"
        "  attribute vec3 a_morphTarget5;\n"
        "  attribute vec3 a_morphTarget6;\n"
        "  attribute vec3 a_morphTarget7;\n"

        "  #ifdef USE_MORPHNORMAL\n"
        "    attribute vec3 a_morphNormal0;\n"
        "    attribute vec3 a_morphNormal1;\n"
        "    attribute vec3 a_morphNormal2;\n"
        "    attribute vec3 a_morphNormal3;\n"
        "  #endif\n"

        "  #ifndef USE_MORPHNORMAL\n"
        "    uniform float u_morphWeights[4];\n"
        "  #else\n"
        "    uniform float u_morphWeights[8];\n"
        "  #endif\n"
        "#endif\n";

std::string morphTargetVertexImp =
        "    #ifdef USE_MORPHTARGET\n"
        "      localPos += (u_morphWeights[0] * a_morphTarget0);\n"
        "      localPos += (u_morphWeights[1] * a_morphTarget1);\n"
        "      localPos += (u_morphWeights[2] * a_morphTarget2);\n"
        "      localPos += (u_morphWeights[3] * a_morphTarget3);\n"
        "      #ifndef USE_MORPHNORMAL\n"
        "        localPos += (u_morphWeights[4] * a_morphTarget4);\n"
        "        localPos += (u_morphWeights[5] * a_morphTarget5);\n"
        "        localPos += (u_morphWeights[6] * a_morphTarget6);\n"
        "        localPos += (u_morphWeights[7] * a_morphTarget7);\n"
        "      #endif\n"
        "    #endif\n"

        "    #ifdef USE_MORPHNORMAL\n"
        "      localNormal += (u_morphWeights[0] * a_morphNormal0);\n"
        "      localNormal += (u_morphWeights[1] * a_morphNormal1);\n"
        "      localNormal += (u_morphWeights[2] * a_morphNormal2);\n"
        "      localNormal += (u_morphWeights[3] * a_morphNormal3);\n"
        "    #endif\n";

#endif //GLES2SHADERMORPHTARGET_H
