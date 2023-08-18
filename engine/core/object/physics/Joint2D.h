//
// (c) 2023 Eduardo Doria.
//

#ifndef JOINT2D_H
#define JOINT2D_H

#include "EntityHandle.h"

namespace Supernova{

    class Joint2D: public EntityHandle{
    public:
        Joint2D(Scene* scene, Entity entity);
        virtual ~Joint2D();

        Joint2D(const Joint2D& rhs);
        Joint2D& operator=(const Joint2D& rhs);

    };
}

#endif //BODY2D_H