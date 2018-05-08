#include "PhysicsWorld2D.h"

#include "Log.h"
#include <Box2D/Box2D.h>

using namespace Supernova;

PhysicsWorld2D::PhysicsWorld2D(): PhysicsWorld(){
    b2Vec2 gravity(0.0f, -10.0f);

    world = new b2World(gravity);

    bodyDef = new b2BodyDef();
    bodyDef->type = b2_dynamicBody;
    bodyDef->position.Set(0.0f, 4.0f);
    body = world->CreateBody(bodyDef);

    dynamicBox = new b2PolygonShape();
    dynamicBox->SetAsBox(1.0f, 1.0f);

    fixtureDef = new b2FixtureDef();
    fixtureDef->shape = dynamicBox;


    fixtureDef->density = 1.0f;
    fixtureDef->friction = 0.3f;


    body->CreateFixture(fixtureDef);

}


void PhysicsWorld2D::step(float timeStep){

    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    world->Step(timeStep, velocityIterations, positionIterations);
    
    b2Vec2 position = body->GetPosition();
    float32 angle = body->GetAngle();

    Log::Print("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
}