// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef MIRROR_H
#define MIRROR_H

#include "Shape.h"

namespace doriax{

    // Flat mesh that reflects the scene, see MirrorComponent. createWall matches
    // the default normal, so a mirror needs no rotation.
    class DORIAX_API Mirror: public Shape{
    public:
        Mirror(Scene* scene);
        Mirror(Scene* scene, Entity entity);
        virtual ~Mirror();

        void setNormal(Vector3 normal);
        void setNormal(const float x, const float y, const float z);
        Vector3 getNormal() const;
    };

}

#endif //MIRROR_H
