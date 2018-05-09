#ifndef PHYSICSWORLD2D_H
#define PHYSICSWORLD2D_H

//
// (c) 2018 Eduardo Doria.
//

#include "PhysicsWorld.h"
#include "Body2D.h"

class b2World;
//class b2BodyDef;
//class b2Body;
//class b2PolygonShape;
//class b2FixtureDef;

namespace Supernova {
    class PhysicsWorld2D: public PhysicsWorld {
    private:
        b2World* world;

        int velocityIterations;
        int positionIterations;
    public:
        PhysicsWorld2D();

        void addBody(Body2D* body);
        void removeBody(Body2D* body);

        void setGravity(Vector2 gravity);
        void setGravity(float gravityX, float gravityY);
        void setVelocityIterations(int velocityIterations);
        void setPositionIterations(int positionIterations);

        Vector2 getGravity();
        int getVelocityIterations();
        int getPositionIterations();

        virtual void step(float timeStep);
    };
}

#endif //PHYSICSWORLD2D_H
