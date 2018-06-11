
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "CollisionShape.h"
#include "Body.h"
#include <vector>
#include "util/FunctionCallback.h"

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class Scene;

    class PhysicsWorld {

        friend class Scene;

    protected:
        std::vector<Body*> bodies;
        bool ownedBodies;
        Scene* attachedScene;

        PhysicsWorld();

    public:
        virtual ~PhysicsWorld();

        FunctionCallback<void(CollisionShape*, CollisionShape*)> onBeginContact;
        FunctionCallback<void(CollisionShape*, CollisionShape*)> onEndContact;

        void setOwnedBodies(bool ownedBodies);
        bool isOwnedBodies();

        void updateBodyObjects();

        virtual void step(float timeStep) = 0;
    };
}


#endif //PHYSICSWORLD_H
