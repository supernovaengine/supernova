//
// (c) 2025 Eduardo Doria.
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
    EntityHandle::operator =(rhs);

    return *this;
}

b2JointId Joint2D::getBox2DJoint() const{
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    return joint.joint;
}

void Joint2D::setDistanceJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB, bool rope){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::DISTANCE;
    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchorOnBodyA;
    joint.anchorB = worldAnchorOnBodyB;
    joint.rope = rope;
    joint.needUpdateJoint = true;
}

void Joint2D::setRevoluteJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::REVOLUTE;
    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.needUpdateJoint = true;
}

void Joint2D::setPrismaticJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor, Vector2 worldAxis){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::PRISMATIC;
    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.axis = worldAxis;
    joint.needUpdateJoint = true;
}

void Joint2D::setMouseJoint(Entity body, Vector2 target){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::MOUSE;
    joint.bodyA = body;
    joint.bodyB = body;
    joint.target = target;
    joint.needUpdateJoint = true;
}

void Joint2D::setWheelJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor, Vector2 worldAxis){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::WHEEL;
    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.axis = worldAxis;
    joint.needUpdateJoint = true;
}

void Joint2D::setWeldJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::WELD;
    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.needUpdateJoint = true;
}

void Joint2D::setMotorJoint(Entity bodyA, Entity bodyB){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();
    joint.type = Joint2DType::MOTOR;
    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.needUpdateJoint = true;
}

Joint2DType Joint2D::getType(){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    return joint.type;
}