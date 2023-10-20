//
// (c) 2023 Eduardo Doria.
//

#include "Body3D.h"

#include "object/Object.h"
#include "subsystem/PhysicsSystem.h"
#include "component/Body3DComponent.h"
#include "util/JoltPhysicsAux.h"

using namespace Supernova;

JPH::EMotionType getBodyTypeToJolt(BodyType type){
    if (type == BodyType::STATIC){
        return JPH::EMotionType::Static;
    }else if (type == BodyType::KINEMATIC){
        return JPH::EMotionType::Kinematic;
    }else if (type == BodyType::DYNAMIC){
        return JPH::EMotionType::Dynamic;
    }

    return JPH::EMotionType::Static;;
}

BodyType getJoltToBodyType(JPH::EMotionType type){
    if (type == JPH::EMotionType::Static){
        return BodyType::STATIC;
    }else if (type == JPH::EMotionType::Kinematic){
        return BodyType::KINEMATIC;
    }else if (type == JPH::EMotionType::Dynamic){
        return BodyType::DYNAMIC;
    }

    return BodyType::STATIC;
}

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

void Body3D::createBoxShape(BodyType type, float width, float height, float depth){
    scene->getSystem<PhysicsSystem>()->createBoxShape3D(entity, type, width, height, depth);
}

void Body3D::createSphereShape(BodyType type, float radius){
    scene->getSystem<PhysicsSystem>()->createSphereShape3D(entity, type, radius);
}

void Body3D::setLinearVelocity(Vector3 linearVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetLinearVelocity(JPH::Vec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
}

Vector3 Body3D::getLinearVelocity() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Vec3 vec = body.body->GetLinearVelocity();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

void Body3D::setType(BodyType type){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.SetMotionType(body.body->GetID(), getBodyTypeToJolt(type), JPH::EActivation::Activate);

    //body.body.

    //body.body->SetMotionType(getBodyTypeToJolt(type), JPH::EActivation::Activate);
}

BodyType Body3D::getType() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return getJoltToBodyType(body.body->GetMotionType());
}

uint32_t Body3D::getID() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->GetID().GetIndex();
}

void Body3D::applyForce(const Vector3& force, const Vector3& point){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->AddForce(JPH::Vec3(force.x, force.y, force.z), JPH::Vec3(point.x, point.y, point.z));
}