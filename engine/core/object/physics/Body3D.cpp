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

void Body3D::setType(BodyType type){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    //TODO: activate?
    body_interface.SetMotionType(body.body->GetID(), getBodyTypeToJolt(type), JPH::EActivation::Activate);
}

BodyType Body3D::getType() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return getJoltToBodyType(body.body->GetMotionType());
}

uint32_t Body3D::getID() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->GetID().GetIndex();
}

bool Body3D::canBeKinematicOrDynamic() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->CanBeKinematicOrDynamic();
}

bool Body3D::isSensor() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->IsSensor();
}

void Body3D::setIsSensor(bool sensor){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetIsSensor(sensor);
}

unsigned int Body3D::getCollisionGroupID() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->GetCollisionGroup().GetGroupID();
}

void Body3D::setCollisionGroupID(unsigned int group){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Ref<JPH::GroupFilterTable> group_filter = new JPH::GroupFilterTable(2);
    group_filter->DisableCollision(0, 0);

    body.body->SetCollisionGroup(JPH::CollisionGroup(group_filter, group, 0));
}

bool Body3D::isAllowSleeping() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->GetAllowSleeping();
}

void Body3D::setAllowSleeping(bool allowSleeping){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetAllowSleeping(allowSleeping);
}

float Body3D::getFriction() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->GetFriction();
}

void Body3D::setFriction(float friction){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetFriction(friction);
}

float Body3D::getRestitution() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    return body.body->GetRestitution();
}

void Body3D::setRestitution(float restitution){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetRestitution(restitution);
}

Vector3 Body3D::getLinearVelocity() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Vec3 vec = body.body->GetLinearVelocity();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

void Body3D::setLinearVelocity(Vector3 linearVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetLinearVelocity(JPH::Vec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
}

void Body3D::setLinearVelocityClamped(Vector3 linearVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetLinearVelocityClamped(JPH::Vec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
}

Vector3 Body3D::getAngularVelocity() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Vec3 vec = body.body->GetAngularVelocity();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

void Body3D::setAngularVelocity(Vector3 angularVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetAngularVelocity(JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

void Body3D::setAngularVelocityClamped(Vector3 angularVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->SetAngularVelocityClamped(JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

Vector3 Body3D::getPointVelocityCOM(Vector3 pointRelativeToCOM){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Vec3 vec = body.body->GetPointVelocityCOM(JPH::Vec3(pointRelativeToCOM.x, pointRelativeToCOM.y, pointRelativeToCOM.z));

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

Vector3 Body3D::getPointVelocity(Vector3 point){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Vec3 vec = body.body->GetPointVelocity(JPH::Vec3(point.x, point.y, point.z));

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}


void Body3D::applyForce(const Vector3& force){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->AddForce(JPH::Vec3(force.x, force.y, force.z));
}

void Body3D::applyForce(const Vector3& force, const Vector3& point){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->AddForce(JPH::Vec3(force.x, force.y, force.z), JPH::Vec3(point.x, point.y, point.z));
}

void Body3D::applyTorque(const Vector3& torque){
    Body3DComponent& body = getComponent<Body3DComponent>();

    body.body->AddTorque(JPH::Vec3(torque.x, torque.y, torque.z));
}