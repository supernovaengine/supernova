//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERSKINNING_H
#define GLES2SHADERSKINNING_H

std::string skinningVertexDec =
        "#ifdef HAS_SKINNING\n"
        "  attribute vec4 a_BoneWeights;\n"
        "  attribute vec4 a_BoneIds;\n"

        "  uniform mat4 u_bonesMatrix[MAXBONES];\n"
        "#endif\n";

std::string skinningVertexImp =
        "    #ifdef HAS_SKINNING\n"
        "      mat4 BoneTransform = u_bonesMatrix[int(a_BoneIds[0])] * a_BoneWeights[0];\n"
        "      BoneTransform += u_bonesMatrix[int(a_BoneIds[1])] * a_BoneWeights[1];\n"
        "      BoneTransform += u_bonesMatrix[int(a_BoneIds[2])] * a_BoneWeights[2];\n"
        "      BoneTransform += u_bonesMatrix[int(a_BoneIds[3])] * a_BoneWeights[3];\n"

        "      localPos = BoneTransform * localPos;\n"
        "    #endif\n";

#endif //GLES2SHADERSKINNING_H
