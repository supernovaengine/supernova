//
// (c) 2023 Eduardo Doria.
//

#include "Joint2D.h"

#include "Scene.h"
#include "subsystem/PhysicsSystem.h"
#include "component/Joint2DComponent.h"

using namespace Supernova;

Joint2D::Joint2D(Scene* scene): EntityHandle(scene){
    addComponent<Joint2DComponent>({});
}

Joint2D::Joint2D(Scene* scene, Entity entity): EntityHandle(scene, entity){

}

Joint2D::~Joint2D(){

}

Joint2D::Joint2D(const Joint2D& rhs): EntityHandle(rhs){
    
}

Joint2D& Joint2D::operator=(const Joint2D& rhs){

    return *this;
}

b2Joint* Joint2D::getBox2DJoint() const{
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    return joint.joint;
}

void Joint2D::setDistanceJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB){
    scene->getSystem<PhysicsSystem>()->loadDistanceJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
}

void Joint2D::setRevoluteJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    scene->getSystem<PhysicsSystem>()->loadRevoluteJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchor);
}

void Joint2D::setPrismaticJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor, Vector2 worldAxis){
    scene->getSystem<PhysicsSystem>()->loadPrismaticJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchor, worldAxis);
}

void Joint2D::setPulleyJoint(Entity bodyA, Entity bodyB, Vector2 groundAnchorA, Vector2 groundAnchorB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB, Vector2 worldAxis, float ratio){
    scene->getSystem<PhysicsSystem>()->loadPulleyJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, groundAnchorA, groundAnchorB, worldAnchorOnBodyA, worldAnchorOnBodyB, worldAxis, ratio);
}

void Joint2D::setGearJoint(Entity bodyA, Entity bodyB, Entity revoluteJoint, Entity prismaticJoint, float ratio){
    scene->getSystem<PhysicsSystem>()->loadGearJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, revoluteJoint, prismaticJoint, ratio);
}

void Joint2D::setMouseJoint(Entity body, Vector2 target){
    scene->getSystem<PhysicsSystem>()->loadMouseJoint2D(getComponent<Joint2DComponent>(), body, target);
}

void Joint2D::setWheelJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor, Vector2 worldAxis){
    scene->getSystem<PhysicsSystem>()->loadWheelJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchor, worldAxis);
}

void Joint2D::setWeldJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    scene->getSystem<PhysicsSystem>()->loadWeldJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchor);
}

void Joint2D::setFrictionJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    scene->getSystem<PhysicsSystem>()->loadFrictionJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchor);
}

void Joint2D::setMotorJoint(Entity bodyA, Entity bodyB){
    scene->getSystem<PhysicsSystem>()->loadMotorJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB);
}

void Joint2D::setRopeJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB){
    scene->getSystem<PhysicsSystem>()->loadRopeJoint2D(getComponent<Joint2DComponent>(), bodyA, bodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
}

Joint2DType Joint2D::getType(){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    return joint.type;
}