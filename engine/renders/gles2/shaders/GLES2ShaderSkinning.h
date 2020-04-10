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
        "      vec4 skinVertex = vec4(localPos, 1.0);\n"
        "      mat4 boneTransform = mat4(0.0);\n"

        "      boneTransform += u_bonesMatrix[int(a_BoneIds[0])] * a_BoneWeights[0];\n"
        "      boneTransform += u_bonesMatrix[int(a_BoneIds[1])] * a_BoneWeights[1];\n"
        "      boneTransform += u_bonesMatrix[int(a_BoneIds[2])] * a_BoneWeights[2];\n"
        "      boneTransform += u_bonesMatrix[int(a_BoneIds[3])] * a_BoneWeights[3];\n"

        "      skinVertex = boneTransform * skinVertex;\n"
        "      localPos = vec3(skinVertex) / skinVertex.w;\n"

        "      #ifdef USE_NORMAL\n"
        "        vec4 skinNormal = vec4(localNormal, 1.0);\n"

        "        skinNormal = boneTransform * skinNormal;\n"
        "        localNormal = vec3(skinNormal) / skinNormal.w;\n"
        "      #endif\n"
        "    #endif\n";

#endif //GLES2SHADERSKINNING_H
