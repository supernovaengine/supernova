//
// (c) 2023 Eduardo Doria.
//

#include "ContactImpulse2D.h"

#include "box2d.h"

using namespace Supernova;

ContactImpulse2D::ContactImpulse2D(const b2ContactImpulse* contactImpulse){
    this->contactImpulse = contactImpulse;
}

ContactImpulse2D::~ContactImpulse2D(){
    this->contactImpulse = contactImpulse;
}

ContactImpulse2D::ContactImpulse2D(const ContactImpulse2D& rhs){
    contactImpulse = rhs.contactImpulse;
}

ContactImpulse2D& ContactImpulse2D::operator=(const ContactImpulse2D& rhs){
    contactImpulse = rhs.contactImpulse;
    
    return *this;
}

const b2ContactImpulse* ContactImpulse2D::getBox2DContactImpulse() const{
    return contactImpulse;
}

int32_t ContactImpulse2D::getCount() const{
    return contactImpulse->count;
}

float ContactImpulse2D::getNormalImpulses(int32_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    return contactImpulse->normalImpulses[index];
}

float ContactImpulse2D::getTangentImpulses(int32_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    return contactImpulse->tangentImpulses[index];
}
