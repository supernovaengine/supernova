
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "CollisionShape.h"
#include "Body.h"
#include <vector>
#include "util/FunctionSubscribe.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class Scene;

    class PhysicsWorld {

        friend class Scene;

    protected:
        std::vector<Body*> bodies;
        Scene* attachedScene;
        bool worldSpace;

        bool ownedBodies;

        PhysicsWorld();

    public:
        virtual ~PhysicsWorld();

        void setOwnedBodies(bool ownedBodies);
        bool isOwnedBodies();

        virtual bool addBody(Body* body);
        virtual void removeBody(Body* body);

        virtual void setGravity(Vector3 gravity) = 0;
        virtual void setGravity(Vector2 gravity) = 0;
        virtual void setGravity(float gravityX, float gravityY) = 0;
        virtual void setGravity(float gravityX, float gravityY, float gravityZ) = 0;

        bool isWorldSpace() const;
        void setWorldSpace(bool worldSpace);

        void updateBodyObjects();

        virtual void step(float timeStep) = 0;
    };
}


#endif //PHYSICSWORLD_H
