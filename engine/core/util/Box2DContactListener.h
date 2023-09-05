
//
// (c) 2023 Eduardo Doria.
//

#ifndef Box2DContactListener_h
#define Box2DContactListener_h

#include "box2d.h"
#include "subsystem/PhysicsSystem.h"
#include "object/physics/Contact2D.h"
#include "object/physics/Manifold2D.h"
#include "object/physics/ContactImpulse2D.h"

namespace Supernova{

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

#endif //Box2DContactListener_h
