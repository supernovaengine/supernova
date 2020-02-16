#ifndef PHYSICSWORLD2D_H
#define PHYSICSWORLD2D_H

//
// (c) 2018 Eduardo Doria.
//

#include "PhysicsWorld.h"
#include "Body2D.h"
#include "Contact2D.h"
#include "Joint2D.h"

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
        float pointsToMeterScale;

        std::vector<Joint2D*> joints;

    public:
        PhysicsWorld2D();
        PhysicsWorld2D(float pointsToMeterScale);
        virtual ~PhysicsWorld2D();
        
        FunctionSubscribe<void(Contact2D*)> onBeginContact;
        FunctionSubscribe<void(Contact2D*)> onPreSolve;
        FunctionSubscribe<void(Contact2D*)> onPostSolve;
        FunctionSubscribe<void(Contact2D*)> onEndContact;

        b2World* getBox2DWorld();

        virtual bool addBody(Body* body);
        virtual void removeBody(Body* body);

        bool addJoint(Joint2D* joint);
        void removeJoint(Joint2D* joint);

        virtual void setGravity(Vector3 gravity);
        virtual void setGravity(Vector2 gravity);
        virtual void setGravity(float gravityX, float gravityY);
        virtual void setGravity(float gravityX, float gravityY, float gravityZ);

        void setVelocityIterations(int velocityIterations);
        void setPositionIterations(int positionIterations);

        Vector2 getGravity();
        int getVelocityIterations();
        int getPositionIterations();

        float getPointsToMeterScale();

        virtual void step(float timeStep);
    };
}

#endif //PHYSICSWORLD2D_H
