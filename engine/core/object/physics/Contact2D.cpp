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

bool Contact2D::isTouching(){
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