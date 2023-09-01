
//
// (c) 2023 Eduardo Doria.
//

#ifndef Box2DContactListener_h
#define Box2DContactListener_h

#include "box2d.h"
#include "subsystem/PhysicsSystem.h"

namespace Supernova{

    class Box2DContactListener : public b2ContactListener{
    private:
        PhysicsSystem* physicsSystem;

    public:

        Box2DContactListener(PhysicsSystem* physicsSystem){
            this->physicsSystem = physicsSystem;
        }
        
        void BeginContact(b2Contact* contact){ 
            physicsSystem->beginContact2D.call();
        }
        
        void EndContact(b2Contact* contact)
        { /* handle end event */ }
        
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
        { /* handle pre-solve event */ }
        
        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
        { /* handle post-solve event */ }
    };

}

#endif //Box2DContactListener_h
