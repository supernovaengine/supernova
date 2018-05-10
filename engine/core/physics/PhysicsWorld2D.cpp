#include "PhysicsWorld2D.h"

#include "Log.h"
#include <Box2D/Box2D.h>

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

PhysicsWorld2D::PhysicsWorld2D(): PhysicsWorld(){
    world = new b2World(b2Vec2(0.0f, 0.0f));

    velocityIterations = 8;
    positionIterations = 3;
}

void PhysicsWorld2D::addBody(Body2D* body){
    body->createBody(this->world);
}

void PhysicsWorld2D::removeBody(Body2D* body){
    world->DestroyBody(body->body);
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