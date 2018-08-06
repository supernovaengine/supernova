//
// (c) 2018 Eduardo Doria.
//

#include "Joint2D.h"

#include "PhysicsWorld2D.h"
#include <Box2D/Box2D.h>

using namespace Supernova;

Joint2D::Joint2D(){
    joint = NULL;
    jointDef = NULL;

    collideConnected = false;

    bodyA = NULL;
    bodyB = NULL;

    type = Joint2DType::REVOLUTE;
}

Joint2D::Joint2D(Joint2DType type): Joint2D(){
    this->type = type;
}

Joint2D::~Joint2D(){

}

void Joint2D::create(PhysicsWorld2D* world){
    if (!joint && bodyA->getBox2DBody() && bodyB->getBox2DBody()) {

        if (type == Joint2DType::REVOLUTE){
            jointDef = new b2RevoluteJointDef();
        }

        jointDef->collideConnected = collideConnected;
        jointDef->bodyA = bodyA->getBox2DBody();
        jointDef->bodyB = bodyB->getBox2DBody();

        if (type == Joint2DType::REVOLUTE) {
            b2RevoluteJointDef *revJointdef = (b2RevoluteJointDef *) jointDef;
            revJointdef->localAnchorA.Set(localAnchorA.x, localAnchorA.y);
            revJointdef->localAnchorB.Set(localAnchorB.x, localAnchorB.y);
        }

        joint = world->getBox2DWorld()->CreateJoint(jointDef);
        joint->SetUserData(this);
        this->world = world;
    }
}

void Joint2D::destroy(){
    if (joint && world) {
        world->getBox2DWorld()->DestroyJoint(joint);
        joint = NULL;
        world = NULL;
    }
}

void Joint2D::setType(Joint2DType type){
    this->type = type;
}

void Joint2D::setBodyA(Body2D* body){
    this->bodyA = body;
    //jointDef->bodyA = body->getBox2DBody();
}

Body2D* Joint2D::getBodyA(){
    return bodyA;
    /*
    Body2D* body = (Body2D*)jointDef->bodyA->GetUserData();
    if (joint)
        body = (Body2D*)joint->GetBodyA()->GetUserData();

    return body;
    */
}

void Joint2D::setBodyB(Body2D* body){
    this->bodyB = body;
    //jointDef->bodyB = body->getBox2DBody();
}

Body2D* Joint2D::getBodyB(){
    return bodyB;
    /*
    Body2D* body = (Body2D*)jointDef->bodyB->GetUserData();
    if (joint)
        body = (Body2D*)joint->GetBodyB()->GetUserData();

    return body;
    */
}

void Joint2D::setCollideConnected(bool collideConnected){
    //jointDef->collideConnected = collideConnected;
    this->collideConnected = collideConnected;
}

bool Joint2D::getCollideConnected(){
    return collideConnected;
    /*
    bool collideConnected = jointDef->collideConnected;
    if (joint)
        collideConnected = joint->GetCollideConnected();

    return collideConnected;
     */
}

const Vector2 &Joint2D::getLocalAnchorA() const {
    return localAnchorA;
}

void Joint2D::setLocalAnchorA(const Vector2 &localAnchorA) {
    Joint2D::localAnchorA = localAnchorA;
}

const Vector2 &Joint2D::getLocalAnchorB() const {
    return localAnchorB;
}

void Joint2D::setLocalAnchorB(const Vector2 &localAnchorB) {
    Joint2D::localAnchorB = localAnchorB;
}
