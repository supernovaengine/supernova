#ifndef PHYSICSWORLD2D_H
#define PHYSICSWORLD2D_H

#include "PhysicsWorld.h"

class b2World;
class b2BodyDef;
class b2Body;
class b2PolygonShape;
class b2FixtureDef;

namespace Supernova {
    class PhysicsWorld2D: public PhysicsWorld {
    private:
        b2World* world;
        b2BodyDef* bodyDef;
        b2Body* body;
        b2PolygonShape* dynamicBox;
        b2FixtureDef* fixtureDef;
    public:
        PhysicsWorld2D();
        virtual void step(float timeStep);
    };
}

#endif //PHYSICSWORLD2D_H
