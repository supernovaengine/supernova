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
    joint.worldAnchorOnBodyA = worldAnchorOnBodyA;
    joint.worldAnchorOnBodyB = worldAnchorOnBodyB;
}