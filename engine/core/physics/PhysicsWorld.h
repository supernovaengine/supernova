
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "Body.h"

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class PhysicsWorld {
    public:
        PhysicsWorld();

        virtual void step(float timeStep) = 0;
    };
}


#endif //PHYSICSWORLD_H
