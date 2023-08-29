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

void Joint2D::setDistanceJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::DISTANCE;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchorOnBodyA;
    joint.anchorB = worldAnchorOnBodyB;

    joint.needUpdate = true;
}

void Joint2D::setRevoluteJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::REVOLUTE;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.anchorB = worldAnchor;

    joint.needUpdate = true;
}

void Joint2D::setPrismaticJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor, Vector2 worldAxis){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::PRISMATIC;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.anchorB = worldAnchor;
    joint.axis = worldAxis;

    joint.needUpdate = true;
}

void Joint2D::setPulleyJoint(Entity bodyA, Entity bodyB, Vector2 groundAnchorA, Vector2 groundAnchorB, Vector2 worldAnchorOnBodyA, Vector2 worldAnchorOnBodyB, Vector2 worldAxis, float ratio){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::PULLEY;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.groundAnchorA = groundAnchorA;
    joint.groundAnchorB = groundAnchorB;
    joint.anchorA = worldAnchorOnBodyA;
    joint.anchorB = worldAnchorOnBodyB;
    joint.ratio = ratio;

    joint.needUpdate = true;
}

void Joint2D::setGearJoint(Entity bodyA, Entity bodyB, Entity revoluteJoint, Entity prismaticJoint, float ratio){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::PULLEY;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.joint1 = revoluteJoint;
    joint.joint2 = prismaticJoint;
    joint.ratio = ratio;

    joint.needUpdate = true;
}

void Joint2D::setMouseJoint(Entity body, Vector2 target){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::MOUSE;

    joint.bodyA = body;
    joint.bodyB = body;
    joint.target = target;

    joint.needUpdate = true;
}

void Joint2D::setWheelJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor, Vector2 worldAxis){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::WHEEL;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.anchorB = worldAnchor;
    joint.axis = worldAxis;

    joint.needUpdate = true;
}

void Joint2D::setWeldJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::WELD;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.anchorB = worldAnchor;

    joint.needUpdate = true;
}

void Joint2D::setFrictionJoint(Entity bodyA, Entity bodyB, Vector2 worldAnchor){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::FRICTION;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;
    joint.anchorA = worldAnchor;
    joint.anchorB = worldAnchor;

    joint.needUpdate = true;
}

void Joint2D::setMotorJoint(Entity bodyA, Entity bodyB){
    Joint2DComponent& joint = getComponent<Joint2DComponent>();

    joint.type = Joint2DType::MOTOR;

    joint.bodyA = bodyA;
    joint.bodyB = bodyB;

    joint.needUpdate = true;
}