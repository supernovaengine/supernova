#ifndef PHYSICSWORLD2D_H
#define PHYSICSWORLD2D_H

//
// (c) 2018 Eduardo Doria.
//

#include "PhysicsWorld.h"
#include "Body2D.h"
#include "Contact2D.h"

class b2World;
class b2ContactListener;

namespace Supernova {
    class PhysicsWorld2D: public PhysicsWorld {
    private:
        b2World* world;

        b2ContactListener* contactListener;

        int velocityIterations;
        int positionIterations;

        //Cannot be changed after creation
        int pointsToMeterScale;

    public:
        PhysicsWorld2D();
        PhysicsWorld2D(int pointsToMeterScale);
        virtual ~PhysicsWorld2D();
        
        FunctionCallback<void(Contact2D*)> onBeginContact;
        FunctionCallback<void(Contact2D*)> onPreSolve;
        FunctionCallback<void(Contact2D*)> onPostSolve;
        FunctionCallback<void(Contact2D*)> onEndContact;

        b2World* getBox2DWorld();

        void addBody(Body2D* body);
        void removeBody(Body2D* body);

        void setGravity(Vector2 gravity);
        void setGravity(float gravityX, float gravityY);
        void setVelocityIterations(int velocityIterations);
        void setPositionIterations(int positionIterations);

        Vector2 getGravity();
        int getVelocityIterations();
        int getPositionIterations();

        int getPointsToMeterScale();

        virtual void step(float timeStep);
    };
}

#endif //PHYSICSWORLD2D_H
