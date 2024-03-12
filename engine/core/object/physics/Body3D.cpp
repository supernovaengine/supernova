//
// (c) 2023 Eduardo Doria.
//

#include "Body3D.h"

#include "object/Object.h"
#include "subsystem/PhysicsSystem.h"
#include "component/Body3DComponent.h"
#include "util/JoltPhysicsAux.h"

#include <iostream>

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

Body3D& Body3D::operator=(const Body3D& rhs){
    EntityHandle::operator =(rhs);

    return *this;
}

void Body3D::checkBody(const Body3DComponent& body) const{
    if (!body.body){
        Log::error("Body3D is not loaded");
        throw std::runtime_error("Body3D is not loaded");;
    }
}

JPH::Body* Body3D::getJoltBody() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body;
}

Object Body3D::getAttachedObject(){
    return Object(scene, entity);
}

void Body3D::load(){
    scene->getSystem<PhysicsSystem>()->loadBody3D(entity);
}

int Body3D::createBoxShape(float width, float height, float depth){
    return scene->getSystem<PhysicsSystem>()->createBoxShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, width, height, depth);
}

int Body3D::createBoxShape(Vector3 position, Quaternion rotation, float width, float height, float depth){
    return scene->getSystem<PhysicsSystem>()->createBoxShape3D(entity, position, rotation, width, height, depth);
}

int Body3D::createSphereShape(float radius){
    return scene->getSystem<PhysicsSystem>()->createSphereShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, radius);
}

int Body3D::createSphereShape(Vector3 position, Quaternion rotation, float radius){
    return scene->getSystem<PhysicsSystem>()->createSphereShape3D(entity, position, rotation, radius);
}

int Body3D::createCapsuleShape(float halfHeight, float radius){
    return scene->getSystem<PhysicsSystem>()->createCapsuleShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, halfHeight, radius);
}

int Body3D::createCapsuleShape(Vector3 position, Quaternion rotation, float halfHeight, float radius){
    return scene->getSystem<PhysicsSystem>()->createCapsuleShape3D(entity, position, rotation, halfHeight, radius);
}

int Body3D::createTaperedCapsuleShape(float halfHeight, float topRadius, float bottomRadius){
    return scene->getSystem<PhysicsSystem>()->createTaperedCapsuleShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, halfHeight, topRadius, bottomRadius);
}

int Body3D::createTaperedCapsuleShape(Vector3 position, Quaternion rotation, float halfHeight, float topRadius, float bottomRadius){
    return scene->getSystem<PhysicsSystem>()->createTaperedCapsuleShape3D(entity, position, rotation, halfHeight, topRadius, bottomRadius);
}

int Body3D::createCylinderShape(float halfHeight, float radius){
    return scene->getSystem<PhysicsSystem>()->createCylinderShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, halfHeight, radius);
}

int Body3D::createCylinderShape(Vector3 position, Quaternion rotation, float halfHeight, float radius){
    return scene->getSystem<PhysicsSystem>()->createCylinderShape3D(entity, position, rotation, halfHeight, radius); 
}

int Body3D::createConvexHullShape(std::vector<Vector3> vertices){
    return scene->getSystem<PhysicsSystem>()->createConvexHullShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, vertices);
}

int Body3D::createConvexHullShape(Vector3 position, Quaternion rotation, std::vector<Vector3> vertices){
    return scene->getSystem<PhysicsSystem>()->createConvexHullShape3D(entity, position, rotation, vertices);
}

int Body3D::createMeshShape(){
    return scene->getSystem<PhysicsSystem>()->createMeshShape3D(entity, getComponent<MeshComponent>());
}

int Body3D::createMeshShape(std::vector<Vector3> vertices, std::vector<uint16_t> indices){
    return scene->getSystem<PhysicsSystem>()->createMeshShape3D(entity, Vector3::ZERO, Quaternion::IDENTITY, vertices, indices);
}

int Body3D::createMeshShape(Vector3 position, Quaternion rotation, std::vector<Vector3> vertices, std::vector<uint16_t> indices){
    return scene->getSystem<PhysicsSystem>()->createMeshShape3D(entity, position, rotation, vertices, indices);
}

int Body3D::createHeightFieldShape(){
    return scene->getSystem<PhysicsSystem>()->createHeightFieldShape3D(entity, getComponent<TerrainComponent>(), 0);
}

int Body3D::createHeightFieldShape(unsigned int samplesSize){
    return scene->getSystem<PhysicsSystem>()->createHeightFieldShape3D(entity, getComponent<TerrainComponent>(), samplesSize);
}

void Body3D::setShapeDensity(float density){
    setShapeDensity(0, density);
}

void Body3D::setShapeDensity(size_t index, float density){
    Body3DComponent& body = getComponent<Body3DComponent>();

    if (index >= 0 && index < body.numShapes){
        JPH::ConvexShape* shape = dynamic_cast<JPH::ConvexShape*>(body.shapes[index].shape);
        if (shape){
            shape->SetDensity(density);
        }else{
            Log::warn("The shape %i does not have density", index);
        }
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

float Body3D::getShapeDensity() const{
    return getShapeDensity(0);
}

float Body3D::getShapeDensity(size_t index) const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    if (index >= 0 && index < body.numShapes){
        JPH::ConvexShape* shape = dynamic_cast<JPH::ConvexShape*>(body.shapes[index].shape);
        if (shape){
            return shape->GetDensity();
        }else{
            Log::warn("The shape %i does not have density", index);
        }
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return 0;
}

void Body3D::setType(BodyType type){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body.type = type;

    if (body.body){
        JPH::EActivation activation = JPH::EActivation::DontActivate;

        if (type != BodyType::STATIC){
            activation = JPH::EActivation::Activate;
        }

        body_interface.SetMotionType(body.body->GetID(), getBodyTypeToJolt(type), activation);
    }
}

BodyType Body3D::getType() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return getJoltToBodyType(body.body->GetMotionType());
}

uint32_t Body3D::getID() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->GetID().GetIndex();
}

bool Body3D::canBeKinematicOrDynamic() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->CanBeKinematicOrDynamic();
}

void Body3D::activate(){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.ActivateBody(body.body->GetID());
}

void Body3D::deactivate(){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.DeactivateBody(body.body->GetID());
}

bool Body3D::isSensor() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->IsSensor();
}

void Body3D::setIsSensor(bool sensor){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetIsSensor(sensor);
}

bool Body3D::isCollideKinematicVsNonDynamic() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->GetCollideKinematicVsNonDynamic();
}

void Body3D::setCollideKinematicVsNonDynamic(bool collideKinematicVsNonDynamic){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetCollideKinematicVsNonDynamic(collideKinematicVsNonDynamic);
}

void Body3D::setAllowedDOFsAll(){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    body.body->GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::All, body.body->GetShape()->GetMassProperties());
}

void Body3D::setAllowedDOFs2DPlane(){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    body.body->GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::Plane2D, body.body->GetShape()->GetMassProperties());
}

void Body3D::setAllowedDOFs(bool translationX, bool translationY, bool translationZ, bool rotationX, bool rotationY, bool rotationZ){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    JPH::EAllowedDOFs allowedDOFs;

    if (translationX){
        allowedDOFs |= JPH::EAllowedDOFs::TranslationX;
    }
    if (translationY){
        allowedDOFs |= JPH::EAllowedDOFs::TranslationY;
    }
    if (translationZ){
        allowedDOFs |= JPH::EAllowedDOFs::TranslationZ;
    }
    if (rotationX){
        allowedDOFs |= JPH::EAllowedDOFs::RotationX;
    }
    if (rotationY){
        allowedDOFs |= JPH::EAllowedDOFs::RotationY;
    }
    if (rotationZ){
        allowedDOFs |= JPH::EAllowedDOFs::RotationZ;
    }

    body.body->GetMotionProperties()->SetMassProperties(allowedDOFs, body.body->GetShape()->GetMassProperties());
}

float Body3D::getMass() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    return body.body->GetShape()->GetMassProperties().mMass;
}

void Body3D::setMass(float mass){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    JPH::MassProperties massProperties = body.body->GetShape()->GetMassProperties();
    massProperties.mMass = mass;
    body.body->GetMotionProperties()->SetMassProperties(body.body->GetMotionProperties()->GetAllowedDOFs(), massProperties);
}

float Body3D::getGravityFactor() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    return body.body->GetMotionProperties()->GetGravityFactor();
}

void Body3D::setGravityFactor(float gravityFactor){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);

    body.body->GetMotionProperties()->SetGravityFactor(gravityFactor);
}

void Body3D::setBitsFilter(uint32_t category, uint32_t mask){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.SetObjectLayer(body.body->GetID(), JPH::ObjectLayerPairFilterMask::sGetObjectLayer(category, mask));
}

uint32_t Body3D::getCategoryBitsFilter() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    JPH::ObjectLayer objectLayer = body_interface.GetObjectLayer(body.body->GetID());

    return JPH::ObjectLayerPairFilterMask::sGetGroup(objectLayer);
}

void Body3D::setCategoryBitsFilter(uint32_t category){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    JPH::ObjectLayer objectLayer = body_interface.GetObjectLayer(body.body->GetID());
    uint32_t mask = JPH::ObjectLayerPairFilterMask::sGetMask(objectLayer);

    body_interface.SetObjectLayer(body.body->GetID(), JPH::ObjectLayerPairFilterMask::sGetObjectLayer(category, mask));
}

uint32_t Body3D::getMaskBitsFilter() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    JPH::ObjectLayer objectLayer = body_interface.GetObjectLayer(body.body->GetID());

    return JPH::ObjectLayerPairFilterMask::sGetMask(objectLayer);
}

void Body3D::setMaskBitsFilter(uint32_t mask){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    JPH::ObjectLayer objectLayer = body_interface.GetObjectLayer(body.body->GetID());
    uint32_t group = JPH::ObjectLayerPairFilterMask::sGetGroup(objectLayer);

    body_interface.SetObjectLayer(body.body->GetID(), JPH::ObjectLayerPairFilterMask::sGetObjectLayer(group, mask));
}

uint32_t Body3D::getCollisionGroupID() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->GetCollisionGroup().GetGroupID();
}

void Body3D::setCollisionGroupID(uint32_t group){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Ref<JPH::GroupFilterTable> group_filter = new JPH::GroupFilterTable(2);
    group_filter->DisableCollision(0, 0);

    body.body->SetCollisionGroup(JPH::CollisionGroup(group_filter, group, 0));
}

bool Body3D::isAllowSleeping() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->GetAllowSleeping();
}

void Body3D::setAllowSleeping(bool allowSleeping){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetAllowSleeping(allowSleeping);
}

float Body3D::getFriction() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->GetFriction();
}

void Body3D::setFriction(float friction){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetFriction(friction);
}

float Body3D::getRestitution() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    return body.body->GetRestitution();
}

void Body3D::setRestitution(float restitution){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetRestitution(restitution);
}

Vector3 Body3D::getLinearVelocity() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetLinearVelocity();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

void Body3D::setLinearVelocity(Vector3 linearVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.SetLinearVelocity(body.body->GetID(), JPH::Vec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
}

void Body3D::setLinearVelocityClamped(Vector3 linearVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetLinearVelocityClamped(JPH::Vec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
}

Vector3 Body3D::getAngularVelocity() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetAngularVelocity();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

void Body3D::setAngularVelocity(Vector3 angularVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.SetAngularVelocity(body.body->GetID(), JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

void Body3D::setAngularVelocityClamped(Vector3 angularVelocity){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    body.body->SetAngularVelocityClamped(JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

Vector3 Body3D::getPointVelocityCOM(Vector3 pointRelativeToCOM) const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetPointVelocityCOM(JPH::Vec3(pointRelativeToCOM.x, pointRelativeToCOM.y, pointRelativeToCOM.z));

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

Vector3 Body3D::getPointVelocity(Vector3 point) const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetPointVelocity(JPH::Vec3(point.x, point.y, point.z));

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

Vector3 Body3D::getAccumulatedForce() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetAccumulatedForce();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

Vector3 Body3D::getAccumulatedTorque() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetAccumulatedTorque();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}

Matrix4 Body3D::getInverseInertia() const{
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Mat44 mat = body.body->GetInverseInertia();

    Matrix4 retMat;
    for (int i = 0; i < 4; i++){
        retMat.setColumn(i, Vector4(mat.GetColumn4(i).GetX(), mat.GetColumn4(i).GetY(), mat.GetColumn4(i).GetZ(), mat.GetColumn4(i).GetW()));
    }

    return retMat;
}

void Body3D::applyForce(const Vector3& force){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.AddForce(body.body->GetID(), JPH::Vec3(force.x, force.y, force.z));
}

void Body3D::applyForce(const Vector3& force, const Vector3& point){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.AddForce(body.body->GetID(), JPH::Vec3(force.x, force.y, force.z), JPH::Vec3(point.x, point.y, point.z));
}

void Body3D::applyTorque(const Vector3& torque){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.AddTorque(body.body->GetID(), JPH::Vec3(torque.x, torque.y, torque.z));
}

void Body3D::applyImpulse(const Vector3& impulse){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.AddImpulse(body.body->GetID(), JPH::Vec3(impulse.x, impulse.y, impulse.z));
}

void Body3D::applyImpulse(const Vector3& impulse, const Vector3& point){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.AddImpulse(body.body->GetID(), JPH::Vec3(impulse.x, impulse.y, impulse.z), JPH::Vec3(point.x, point.y, point.z));
}

void Body3D::applyAngularImpulse(const Vector3& angularImpulse){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();
    JPH::BodyInterface &body_interface = world->GetBodyInterface();

    body_interface.AddAngularImpulse(body.body->GetID(), JPH::Vec3(angularImpulse.x, angularImpulse.y, angularImpulse.z));
}

bool Body3D::applyBuoyancyImpulse(const Vector3& surfacePosition, const Vector3& surfaceNormal, const float buoyancy, const float linearDrag, const float angularDrag, const Vector3& fluidVelocity, const Vector3& gravity, const float deltaTime){
    Body3DComponent& body = getComponent<Body3DComponent>();

    JPH::Vec3 surPos = JPH::Vec3(surfacePosition.x, surfacePosition.y, surfacePosition.z);
    JPH::Vec3 surNor = JPH::Vec3(surfaceNormal.x, surfaceNormal.y, surfaceNormal.z);
    JPH::Vec3 fluVel = JPH::Vec3(fluidVelocity.x, fluidVelocity.y, fluidVelocity.z);
    JPH::Vec3 grav = JPH::Vec3(gravity.x, gravity.y, gravity.z);

    checkBody(body);
    return body.body->ApplyBuoyancyImpulse(surPos, surNor, buoyancy, linearDrag, angularDrag, fluVel, grav, deltaTime);
}

Vector3 Body3D::getCenterOfMassPosition(){
    Body3DComponent& body = getComponent<Body3DComponent>();

    checkBody(body);
    JPH::Vec3 vec = body.body->GetCenterOfMassPosition();

    return Vector3(vec.GetX(), vec.GetY(), vec.GetZ());
}