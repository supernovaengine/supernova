
//
// (c) 2018 Eduardo Doria.
//

#include "Contact2D.h"

#include <Box2D/Box2D.h>

using namespace Supernova;

Contact2D::Contact2D(b2Contact* contact){
    this->contact = contact;
    worldManifold = new b2WorldManifold();
}

Contact2D::Contact2D(const Contact2D& c){
    this->contact = c.contact;
}

Contact2D& Contact2D::operator = (const Contact2D& c){
    this->contact = c.contact;

    return *this;
}

Contact2D::~Contact2D(){
    delete worldManifold;
}

CollisionShape2D* Contact2D::getCollisionShapeA(){
    return (CollisionShape2D*)contact->GetFixtureA()->GetUserData();
}

CollisionShape2D* Contact2D::getCollisionShapeB(){
    return (CollisionShape2D*)contact->GetFixtureB()->GetUserData();
}

Vector2 Contact2D::getNormal(){
    contact->GetWorldManifold( worldManifold );
    b2Vec2 normal = worldManifold->normal;
    return Vector2(normal.x, normal.y);
}

Vector2 Contact2D::getPoint(int index){
    contact->GetWorldManifold( worldManifold );
    b2Vec2 point = worldManifold->points[index];
    return Vector2(point.x, point.y);
}

int Contact2D::getNumPoints(){
    return contact->GetManifold()->pointCount;
}

bool Contact2D::isTouching(){
    return contact->IsTouching();
}

bool Contact2D::isEnabled(){
    return contact->IsEnabled();
}

void Contact2D::setEnabled(bool enabled){
    contact->SetEnabled(enabled);
}

float Contact2D::getFriction(){
    return contact->GetFriction();
}

void Contact2D::setFriction(float friction){
    contact->SetFriction(friction);
}

float Contact2D::getRestitution(){
    return contact->GetRestitution();
}

void Contact2D::setRestitution(float restitution){
    contact->SetRestitution(restitution);
}

float Contact2D::getTangentSpeed(){
    return contact->GetTangentSpeed();
}

void Contact2D::setTangentSpeed(float tangetSpeed){
    contact->SetTangentSpeed(tangetSpeed);
}


