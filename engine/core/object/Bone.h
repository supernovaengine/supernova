//
// (c) 2024 Eduardo Doria.
//

#ifndef BONE_H
#define BONE_H

#include "Object.h"

namespace Supernova{

    class Bone: public Object{
    public:
        Bone(Scene* scene, Entity entity);
        virtual ~Bone();

        Bone(const Bone& rhs);
        Bone& operator=(const Bone& rhs);
    };
}

#endif //BONE_H