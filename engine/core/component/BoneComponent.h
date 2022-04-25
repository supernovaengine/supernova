//
// (c) 2022 Eduardo Doria.
//

#ifndef BONE_COMPONENT_H
#define BONE_COMPONENT_H

#include <string>

namespace Supernova{

    struct BoneComponent{
        Entity model = NULL_ENTITY;

        int index;
        std::string name;

        Vector3 bindPosition;
        Quaternion bindRotation;
        Vector3 bindScale;

        Matrix4 offsetMatrix; // inverse bind matrix
    };

}

#endif //BONE_COMPONENT_H