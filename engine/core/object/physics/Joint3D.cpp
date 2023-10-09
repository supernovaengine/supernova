//
// (c) 2023 Eduardo Doria.
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

    return *this;
}

JPH::TwoBodyConstraint* Joint3D::getJoltJoint() const{
    Joint3DComponent& joint = getComponent<Joint3DComponent>();

    return joint.joint;
}

void Joint3D::setDistanceJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchorOnBodyA, Vector3 worldAnchorOnBodyB){
    //scene->getSystem<PhysicsSystem>()->loadDistanceJoint3D(getComponent<Joint3DComponent>(), bodyA, bodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
}