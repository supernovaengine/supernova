#include "PhysicsWorld2D.h"

#include "Log.h"
#include "CollisionShape.h"
#include <Box2D/Box2D.h>

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

class World2DContactListener: public b2ContactListener {
private:
    PhysicsWorld2D* world;

public:
    World2DContactListener(PhysicsWorld2D* world){
        this->world = world;
    }

    virtual void BeginContact(b2Contact* contact){
        Contact2D contact2D(contact);
        world->onBeginContact.call(&contact2D);
    }

    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold){
        Contact2D contact2D(contact);
        world->onPreSolve.call(&contact2D);
    }

    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){
        Contact2D contact2D(contact);
        world->onPostSolve.call(&contact2D);
    }

    virtual void EndContact(b2Contact* contact){
        Contact2D contact2D(contact);
        world->onEndContact.call(&contact2D);
    }

};


PhysicsWorld2D::PhysicsWorld2D(): PhysicsWorld(){
    world = new b2World(b2Vec2(0.0f, 0.0f));

    contactListener = new World2DContactListener(this);
    world->SetContactListener(contactListener);

    velocityIterations = 8;
    positionIterations = 3;

    pointsToMeterScale = 64;
}

PhysicsWorld2D::PhysicsWorld2D(float pointsToMeterScale): PhysicsWorld2D(){
    this->pointsToMeterScale = pointsToMeterScale;
}

PhysicsWorld2D::~PhysicsWorld2D(){
    for (int i = 0; i < bodies.size(); i++){
        ((Body2D*)bodies[i])->destroyBody();
    }
    delete contactListener;
    delete world;
}

b2World* PhysicsWorld2D::getBox2DWorld(){
    return world;
}

bool PhysicsWorld2D::addBody(Body* body){
    if (PhysicsWorld::addBody(body)){
        ((Body2D*)body)->createBody((PhysicsWorld2D*)this);

        return true;
    }

    return false;
}

void PhysicsWorld2D::removeBody(Body* body){
    PhysicsWorld::removeBody(body);

    ((Body2D*)body)->destroyBody();
}

void PhysicsWorld2D::setGravity(Vector3 gravity){
    world->SetGravity(b2Vec2(gravity.x, gravity.y));
}

void PhysicsWorld2D::setGravity(Vector2 gravity){
    setGravity(Vector3(gravity.x, gravity.y, 0));
}

void PhysicsWorld2D::setGravity(float gravityX, float gravityY){
    setGravity(Vector3(gravityX, gravityY, 0));
}

void PhysicsWorld2D::setGravity(float gravityX, float gravityY, float gravityZ){
    setGravity(Vector3(gravityX, gravityY, gravityZ));
}

void PhysicsWorld2D::setVelocityIterations(int velocityIterations){
    this->velocityIterations = velocityIterations;
}

void PhysicsWorld2D::setPositionIterations(int positionIterations){
    this->positionIterations = positionIterations;
}

Vector2 PhysicsWorld2D::getGravity(){
    b2Vec2 gravity = world->GetGravity();
    return Vector2(gravity.x, gravity.y);
}

int PhysicsWorld2D::getVelocityIterations(){
    return velocityIterations;
}

int PhysicsWorld2D::getPositionIterations(){
    return positionIterations;
}

float PhysicsWorld2D::getPointsToMeterScale(){
    return pointsToMeterScale;
}

void PhysicsWorld2D::step(float timeStep){
    world->Step(timeStep, velocityIterations, positionIterations);
}
