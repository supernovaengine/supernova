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

PhysicsWorld2D::PhysicsWorld2D(int pointsToMeterScale): PhysicsWorld2D(){
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

void PhysicsWorld2D::addBody(Body2D* body){
    bool founded = false;

    std::vector<Body*>::iterator it;
    for (it = bodies.begin(); it != bodies.end(); ++it) {
        if (body == (*it))
            founded = true;
    }

    if (!founded){
        bodies.push_back(body);

        body->createBody(this);
    }
}

void PhysicsWorld2D::removeBody(Body2D* body){
    std::vector<Body*>::iterator i = std::remove(bodies.begin(), bodies.end(), body);
    bodies.erase(i, bodies.end());

    body->destroyBody();
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

int PhysicsWorld2D::getPointsToMeterScale(){
    return pointsToMeterScale;
}

void PhysicsWorld2D::step(float timeStep){
    world->Step(timeStep, velocityIterations, positionIterations);
}
