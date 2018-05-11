#include "PhysicsWorld2D.h"

#include "Log.h"
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
        Body* bodyA = (Body*)contact->GetFixtureA()->GetBody()->GetUserData();
        Body* bodyB = (Body*)contact->GetFixtureB()->GetBody()->GetUserData();

        world->call_onBeginContact(bodyA, bodyB);
    }

    virtual void EndContact(b2Contact* contact){
        Body* bodyA = (Body*)contact->GetFixtureA()->GetBody()->GetUserData();
        Body* bodyB = (Body*)contact->GetFixtureB()->GetBody()->GetUserData();

        world->call_onEndContact(bodyA, bodyB);
    }

};


PhysicsWorld2D::PhysicsWorld2D(): PhysicsWorld(){
    world = new b2World(b2Vec2(0.0f, 0.0f));

    contactListener = new World2DContactListener(this);
    world->SetContactListener(contactListener);

    pointToMeterRatio = 32;

    velocityIterations = 8;
    positionIterations = 3;
}

PhysicsWorld2D::~PhysicsWorld2D(){
    delete contactListener;
    delete world;
}

void PhysicsWorld2D::addBody(Body2D* body){
    body->createBody(this->world);
}

void PhysicsWorld2D::removeBody(Body2D* body){
    world->DestroyBody(body->body);
}

void PhysicsWorld2D::setPointToMeterRatio(int pointToMeterRatio){
    this->pointToMeterRatio = pointToMeterRatio;
}

int PhysicsWorld2D::getPointToMeterRatio(){
    return pointToMeterRatio;
}

void PhysicsWorld2D::setGravity(Vector2 gravity){
    setGravity(gravity.x, gravity.y);
}

void PhysicsWorld2D::setGravity(float gravityX, float gravityY){
    world->SetGravity(b2Vec2(gravityX, gravityY));
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

void PhysicsWorld2D::step(float timeStep){
    world->Step(timeStep, velocityIterations, positionIterations);
}