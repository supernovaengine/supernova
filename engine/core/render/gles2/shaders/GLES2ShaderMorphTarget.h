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
        "  #ifndef USE_MORPHNORMAL\n"
        "    uniform float u_morphWeights[4];\n"
        "  #else\n"
        "    uniform float u_morphWeights[8];\n"
        "  #endif\n"
        "#endif\n";

std::string morphTargetVertexImp =
        "    #ifdef USE_MORPHTARGET\n"
        "      vec3 morphPosition = vec3(localPos);\n"
        "      morphPosition += (u_morphWeights[0] * a_morphTarget0);\n"
        "      morphPosition += (u_morphWeights[1] * a_morphTarget1);\n"
        "      morphPosition += (u_morphWeights[2] * a_morphTarget2);\n"
        "      morphPosition += (u_morphWeights[3] * a_morphTarget3);\n"
        "      #ifndef USE_MORPHNORMAL\n"
        "        morphPosition += (u_morphWeights[4] * a_morphTarget4);\n"
        "        morphPosition += (u_morphWeights[5] * a_morphTarget5);\n"
        "        morphPosition += (u_morphWeights[6] * a_morphTarget6);\n"
        "        morphPosition += (u_morphWeights[7] * a_morphTarget7);\n"
        "      #endif\n"
        "      localPos = vec4(morphPosition, localPos.w);\n"
        "    #endif\n";

#endif //GLES2SHADERMORPHTARGET_H
