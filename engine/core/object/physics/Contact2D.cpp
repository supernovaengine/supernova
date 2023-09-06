//
// (c) 2023 Eduardo Doria.
//

#include "Contact2D.h"

#include "box2d.h"

using namespace Supernova;

Contact2D::Contact2D(Scene* scene, b2Contact* contact){
    this->scene = scene;
    this->contact = contact;
}

Contact2D::~Contact2D(){

}

Contact2D::Contact2D(const Contact2D& rhs){
    scene = rhs.scene;
    contact = rhs.contact;

}

Contact2D& Contact2D::operator=(const Contact2D& rhs){
    scene = rhs.scene;
    contact = rhs.contact;
    
    return *this;
}

b2Contact* Contact2D::getBox2DContact() const{
    return contact;
}

Manifold2D Contact2D::getManifold() const{
    return Manifold2D(scene, contact->GetManifold());
}

WorldManifold2D Contact2D::getWorldManifold() const{
    WorldManifold2D worldManifold(scene);
    contact->GetWorldManifold(worldManifold.getBox2DWorldManifold());
    return worldManifold;
}

bool Contact2D::isTouching() const{
    return contact->IsTouching();
}

Entity Contact2D::getBodyEntityA() const{
    return contact->GetFixtureA()->GetBody()->GetUserData().pointer;
}

Body2D Contact2D::getBodyA() const{
    Body2D body2d(scene, getBodyEntityA());

    return body2d;
}

size_t Contact2D::getShapeIndexA() const{
    return contact->GetFixtureA()->GetUserData().pointer;
}

Entity Contact2D::getBodyEntityB() const{
    return contact->GetFixtureB()->GetBody()->GetUserData().pointer;
}

Body2D Contact2D::getBodyB() const{
    Body2D body2d(scene, getBodyEntityB());

    return body2d;
}

size_t Contact2D::getShapeIndexB() const{
    return contact->GetFixtureB()->GetUserData().pointer;
}

bool Contact2D::isEnabled() const{
    return contact->IsEnabled();
}

void Contact2D::setEnabled(bool enabled){
    contact->SetEnabled(enabled);
}

float Contact2D::getFriction() const{
    return contact->GetFriction();
}

void Contact2D::setFriction(float friction){
    contact->SetFriction(friction);
}

void Contact2D::resetFriction(){
    contact->ResetFriction();
}

float Contact2D::getRestitution() const{
    return contact->GetRestitution();
}

void Contact2D::setRestitution(float restitution){
    contact->SetRestitution(restitution);
}

void Contact2D::resetRestitution(){
    contact->ResetRestitution();
}

float Contact2D::getTangentSpeed() const{
    return contact->GetTangentSpeed();
}

void Contact2D::setTangentSpeed(float tangentSpeed){
    contact->SetTangentSpeed(tangentSpeed);
}