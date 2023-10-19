//
// (c) 2023 Eduardo Doria.
//

#include "Contact3D.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/ContactListener.h"

using namespace Supernova;

Contact3D::Contact3D(Scene* scene, const JPH::ContactManifold* contactManifold){
    this->scene = scene;
    this->contactManifold = contactManifold;
}

Contact3D::~Contact3D(){

}

Contact3D::Contact3D(const Contact3D& rhs){
    scene = rhs.scene;
    contactManifold = rhs.contactManifold;

}

Contact3D& Contact3D::operator=(const Contact3D& rhs){
    scene = rhs.scene;
    contactManifold = rhs.contactManifold;
    
    return *this;
}

const JPH::ContactManifold* Contact3D::getJoltContactManifold() const{
    return contactManifold;
}