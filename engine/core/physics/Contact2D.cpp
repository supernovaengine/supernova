
//
// (c) 2018 Eduardo Doria.
//

#include "Contact2D.h"

#include <Box2D/Box2D.h>

using namespace Supernova;

Contact2D::Contact2D(b2Contact* contact){
    this->contact = contact;
}

Contact2D::Contact2D(const Contact2D& c){
    this->contact = c.contact;
}

Contact2D& Contact2D::operator = (const Contact2D& c){
    this->contact = c.contact;

    return *this;
}

Contact2D::~Contact2D(){

}

CollisionShape2D* Contact2D::getCollisionShapeA(){
    return (CollisionShape2D*)contact->GetFixtureA()->GetUserData();
}

CollisionShape2D* Contact2D::getCollisionShapeB(){
    return (CollisionShape2D*)contact->GetFixtureB()->GetUserData();
}


