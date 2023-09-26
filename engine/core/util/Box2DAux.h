
//
// (c) 2023 Eduardo Doria.
//

#ifndef Box2DAux_h
#define Box2DAux_h

#include "box2d.h"
#include "subsystem/PhysicsSystem.h"

namespace Supernova{

    class Box2DContactFilter : public b2ContactFilter{
    private:
        Scene* scene;
        PhysicsSystem* physicsSystem;

    public:

        Box2DContactFilter(Scene* scene, PhysicsSystem* physicsSystem){
            this->scene = scene;
            this->physicsSystem = physicsSystem;
        }
        
        bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB){
            size_t shapeIndexA = fixtureA->GetUserData().pointer;
            Entity entityA = fixtureA->GetBody()->GetUserData().pointer;
            Body2D bodyA(scene, entityA);

            size_t shapeIndexB = fixtureB->GetUserData().pointer;
            Entity entityB = fixtureB->GetBody()->GetUserData().pointer;
            Body2D bodyB(scene, entityB);

            return physicsSystem->shouldCollide2D.callRet(bodyA, shapeIndexA, bodyB, shapeIndexB, true);
        }
    };


    class Box2DContactListener : public b2ContactListener{
    private:
        Scene* scene;
        PhysicsSystem* physicsSystem;

    public:

        Box2DContactListener(Scene* scene, PhysicsSystem* physicsSystem){
            this->scene = scene;
            this->physicsSystem = physicsSystem;
        }
        
        void BeginContact(b2Contact* contact){
            physicsSystem->beginContact2D.call(Contact2D(scene, contact));
        }
        
        void EndContact(b2Contact* contact){
            physicsSystem->endContact2D.call(Contact2D(scene, contact));
        }
        
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold){
            physicsSystem->preSolve2D.call(Contact2D(scene, contact), Manifold2D(scene, oldManifold));
        }
        
        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){
            physicsSystem->postSolve2D.call(Contact2D(scene, contact), ContactImpulse2D(impulse));
        }
    };

}

#endif //Box2DAux_h
