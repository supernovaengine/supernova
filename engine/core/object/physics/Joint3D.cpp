//
// (c) 2025 Eduardo Doria.
//

#include "Joint3D.h"

#include "Scene.h"
#include "subsystem/PhysicsSystem.h"
#include "component/Joint3DComponent.h"

using namespace Supernova;

Joint3D::Joint3D(Scene* scene): EntityHandle(scene){
    addComponent<Joint3DComponent>({});
}

Joint3D::Joint3D(Scene* scene, Entity entity): EntityHandle(scene, entity){

}

Joint3D::~Joint3D(){

}

Joint3D::Joint3D(const Joint3D& rhs): EntityHandle(rhs){
    
}

Joint3D& Joint3D::operator=(const Joint3D& rhs){
    EntityHandle::operator =(rhs);

    return *this;
}

JPH::TwoBodyConstraint* Joint3D::getJoltJoint() const{
    Joint3DComponent& joint = getComponent<Joint3DComponent>();

    return joint.joint;
}

void Joint3D::setFixedJoint(Entity bodyA, Entity bodyB){
    scene->getSystem<PhysicsSystem>()->loadFixedJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB);
}

void Joint3D::setDistanceJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchorOnBodyA, Vector3 worldAnchorOnBodyB){
    scene->getSystem<PhysicsSystem>()->loadDistanceJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
}

void Joint3D::setPointJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchor){
    scene->getSystem<PhysicsSystem>()->loadPointJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchor);
}

void Joint3D::setHingeJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchor, Vector3 axis, Vector3 normal){
    scene->getSystem<PhysicsSystem>()->loadHingeJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchor, axis, normal);
}

void Joint3D::setConeJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchor, Vector3 twistAxis){
    scene->getSystem<PhysicsSystem>()->loadConeJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchor, twistAxis);
}

void Joint3D::setPrismaticJoint(Entity bodyA, Entity bodyB, Vector3 sliderAxis, float limitsMin, float limitsMax){
    scene->getSystem<PhysicsSystem>()->loadPrismaticJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, sliderAxis, limitsMin, limitsMax);
}

void Joint3D::setSwingTwistJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchor, Vector3 twistAxis, Vector3 planeAxis, float normalHalfConeAngle, float planeHalfConeAngle, float twistMinAngle, float twistMaxAngle){
    scene->getSystem<PhysicsSystem>()->loadSwingTwistJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchor, twistAxis, planeAxis, normalHalfConeAngle, planeHalfConeAngle, twistMinAngle, twistMaxAngle);
}

void Joint3D::setSixDOFJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchorOnBodyA, Vector3 worldAnchorOnBodyB, Vector3 axisX, Vector3 axisY){
    scene->getSystem<PhysicsSystem>()->loadSixDOFJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchorOnBodyA, worldAnchorOnBodyB, axisX, axisY);
}

void Joint3D::setPathJoint(Entity bodyA, Entity bodyB, std::vector<Vector3> positions, std::vector<Vector3> tangents, std::vector<Vector3> normals, Vector3 pathPosition, bool isLooping){
    scene->getSystem<PhysicsSystem>()->loadPathJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, positions, tangents, normals, pathPosition, isLooping);
}

void Joint3D::setGearJoint(Entity bodyA, Entity bodyB, Entity hingeA, Entity hingeB, int numTeethGearA, int numTeethGearB){
    scene->getSystem<PhysicsSystem>()->loadGearJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, hingeA, hingeB, numTeethGearA, numTeethGearB);
}

void Joint3D::setRackAndPinionJoint(Entity bodyA, Entity bodyB, Entity hinge, Entity slider, int numTeethRack, int numTeethGear, int rackLength){
    scene->getSystem<PhysicsSystem>()->loadRackAndPinionJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, hinge, slider, numTeethRack, numTeethGear, rackLength);
}

void Joint3D::setPulleyJoint(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 worldAnchorOnBodyA, Vector3 worldAnchorOnBodyB, Vector3 fixedPointA, Vector3 fixedPointB){
    scene->getSystem<PhysicsSystem>()->loadPulleyJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchorOnBodyA, worldAnchorOnBodyB, fixedPointA, fixedPointB);
}

Joint3DType Joint3D::getType(){
    Joint3DComponent& joint = getComponent<Joint3DComponent>();

    return joint.type;
}