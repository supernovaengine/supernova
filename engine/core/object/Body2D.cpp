//
// (c) 2023 Eduardo Doria.
//

#include "Body2D.h"

#include "subsystem/PhysicsSystem.h"
#include "component/Body2DComponent.h"

using namespace Supernova;

Body2D::Body2D(Scene* scene, Entity entity): EntityHandle(scene, entity){

}

Body2D::~Body2D(){

}

Body2D::Body2D(const Body2D& rhs): EntityHandle(rhs){
    
}

Body2D& Body2D::operator=(const Body2D& rhs){

    return *this;
}

int Body2D::createRectShape2D(float width, float height){
    return scene->getSystem<PhysicsSystem>()->addRectShape2D(entity, width, height);
}

void Body2D::setShape2DDensity(size_t index, float density){
    scene->getSystem<PhysicsSystem>()->setShape2DDensity(entity, index, density);
}

void Body2D::setShape2DFriction(size_t index, float friction){
    scene->getSystem<PhysicsSystem>()->setShape2DFriction(entity, index, friction);
}

void Body2D::setShape2DRestitution(size_t index, float restitution){
    scene->getSystem<PhysicsSystem>()->setShape2DRestitution(entity, index, restitution);
}