//
// (c) 2023 Eduardo Doria.
//

#include "Body3D.h"

#include "object/Object.h"
#include "subsystem/PhysicsSystem.h"
#include "component/Body3DComponent.h"
#include "util/JoltPhysicsAux.h"

using namespace Supernova;

Body3D::Body3D(Scene* scene, Entity entity): EntityHandle(scene, entity){

}

Body3D::~Body3D(){

}

Body3D::Body3D(const Body3D& rhs): EntityHandle(rhs){
    
}

JPH::Body* Body3D::getJoltBody() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body;
}

Object Body3D::getAttachedObject(){
    return Object(scene, entity);
}

void Body3D::createBoxShape(float width, float height, float depth){
    scene->getSystem<PhysicsSystem>()->createBoxShape3D(entity, width, height, depth);
}

void Body3D::createSphereShape(){
    scene->getSystem<PhysicsSystem>()->createSphereShape3D(entity);
}