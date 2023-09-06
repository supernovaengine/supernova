
//
// (c) 2023 Eduardo Doria.
//

#ifndef Box2DContactFilter_h
#define Box2DContactFilter_h

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

}

#endif //Box2DContactFilter_h
