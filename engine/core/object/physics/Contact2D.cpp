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

b2Contact* Contact2D::getBox2DContact(){
    return contact;
}

Manifold2D Contact2D::getManifold(){
    return Manifold2D(scene, contact->GetManifold());
}

WorldManifold2D Contact2D::getWorldManifold(){
    return WorldManifold2D(scene, contact->GetWorldManifold());
}

bool Contact2D::isTouching() const{
    return contact->IsTouching();
}

Entity Contact2D::getBodyA(){
    return contact->GetFixtureA()->GetBody()->GetUserData().pointer;
}

Body2D Contact2D::getBodyObjectA(){
    Body2D body2d(scene, getBodyA());

    return body2d;
}

size_t Contact2D::getFixtureIndexA(){
    return contact->GetFixtureA()->GetUserData().pointer;
}

Entity Contact2D::getBodyB(){
    return contact->GetFixtureB()->GetBody()->GetUserData().pointer;
}

Body2D Contact2D::getBodyObjectB(){
    Body2D body2d(scene, getBodyB());

    return body2d;
}

size_t Contact2D::getFixtureIndexB(){
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