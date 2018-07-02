//
// (c) 2018 Eduardo Doria.
//

#include "Joint2D.h"

#include "PhysicsWorld2D.h"
#include <Box2D/Box2D.h>

using namespace Supernova;

Joint2D::Joint2D(){

}

Joint2D::~Joint2D(){

}

void Joint2D::createJoint(PhysicsWorld2D* world){
    joint = world->getBox2DWorld()->CreateJoint(jointDef);
    joint->SetUserData(this);
    this->world = world;
}

void Joint2D::destroyJoint(){
    if (joint && world) {
        world->getBox2DWorld()->DestroyJoint(joint);
        joint = NULL;
        world = NULL;
    }
}

void Joint2D::setBodyA(Body2D* body){
    jointDef->bodyA = body->getBox2DBody();
}

Body2D* Joint2D::getBodyA(){
    Body2D* body = (Body2D*)jointDef->bodyA->GetUserData();
    if (joint)
        body = (Body2D*)joint->GetBodyA()->GetUserData();

    return body;
}

void Joint2D::setBodyB(Body2D* body){
    jointDef->bodyB = body->getBox2DBody();
}

Body2D* Joint2D::getBodyB(){
    Body2D* body = (Body2D*)jointDef->bodyB->GetUserData();
    if (joint)
        body = (Body2D*)joint->GetBodyB()->GetUserData();

    return body;
}

void Joint2D::setCollideConnected(bool collideConnected){
    jointDef->collideConnected = collideConnected;
}

bool Joint2D::getCollideConnected(){
    bool collideConnected = jointDef->collideConnected;
    if (joint)
        collideConnected = joint->GetCollideConnected();

    return collideConnected;
}
