
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

namespace Supernova {
    class PhysicsWorld {
    public:
        PhysicsWorld();
        virtual void step(float timeStep) = 0;
    };
}


#endif //PHYSICSWORLD_H
