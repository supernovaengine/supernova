
//
// (c) 2024 Eduardo Doria.
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
            unsigned long shapeIndexA = fixtureA->GetUserData().pointer;
            Entity entityA = fixtureA->GetBody()->GetUserData().pointer;
            Body2D bodyA(scene, entityA);

            unsigned long shapeIndexB = fixtureB->GetUserData().pointer;
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


    struct B2_API Box2DWorldRayCastOutput{
        b2Fixture* fixture;
        b2Vec2 point;
	    b2Vec2 normal;
	    float fraction;
    };


    class Box2DRayCastCallback : public b2RayCastCallback{
    private:

        std::vector<Box2DWorldRayCastOutput>* outputs;
        bool returnAllResults;
        bool onlyStatic;
        uint16_t collisionGroup;
        uint16_t collisionMask;

	public:

        Box2DRayCastCallback(std::vector<Box2DWorldRayCastOutput>* outputs){
            this->outputs = outputs;
            this->returnAllResults = true;
            this->onlyStatic = false;
            this->collisionGroup = (uint16_t)~0u;
            this->collisionMask = (uint16_t)~0u;
        }

        Box2DRayCastCallback(std::vector<Box2DWorldRayCastOutput>* outputs, bool returnAllResults){
            this->outputs = outputs;
            this->returnAllResults = returnAllResults;
            this->onlyStatic = false;
            this->collisionGroup = (uint16_t)~0u;
            this->collisionMask = (uint16_t)~0u;
        }

        Box2DRayCastCallback(std::vector<Box2DWorldRayCastOutput>* outputs, bool returnAllResults, bool onlyStatic){
            this->outputs = outputs;
            this->returnAllResults = returnAllResults;
            this->onlyStatic = onlyStatic;
            this->collisionGroup = (uint16_t)~0u;
            this->collisionMask = (uint16_t)~0u;
        }

        Box2DRayCastCallback(std::vector<Box2DWorldRayCastOutput>* outputs, bool returnAllResults, bool onlyStatic, uint16_t collisionGroup, uint16_t collisionMask){
            this->outputs = outputs;
            this->returnAllResults = returnAllResults;
            this->onlyStatic = onlyStatic;
            this->collisionGroup = collisionGroup;
            this->collisionMask = collisionMask;
        }

		float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction){
            if (fixture->IsSensor())
                return -1.f;

            if (onlyStatic && fixture->GetBody()->GetType() != b2BodyType::b2_staticBody)
                return -1.f;

            if ((fixture->GetFilterData().categoryBits & collisionMask) && (fixture->GetFilterData().maskBits & collisionGroup)){
                // fraction return to fint the closest intersection
                // return 1 to store all intersections
                // https://www.iforce2d.net/b2dtut/world-querying
                if (returnAllResults){
                    outputs->push_back({fixture, point, normal, fraction});
                    return 1.0f;
                }else{
                    outputs->clear();
                    outputs->push_back({fixture, point, normal, fraction});
                    return fraction;
                }

            }

            return -1.f;
        }
    };

}

#endif //Box2DAux_h
