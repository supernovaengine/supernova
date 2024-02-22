//
// (c) 2023 Eduardo Doria.
//

#include "EntityHandle.h"

using namespace Supernova;

EntityHandle::EntityHandle(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    this->entityOwned = true;
}

EntityHandle::EntityHandle(Scene* scene, Entity entity){
    this->scene = scene;
    this->entity = entity;
    this->entityOwned = false;
}

EntityHandle::~EntityHandle(){
    if (scene && entityOwned)
        scene->destroyEntity(entity);
}

EntityHandle::EntityHandle(const EntityHandle& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;
}

EntityHandle& EntityHandle::operator=(const EntityHandle& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;

    return *this;
}

Scene* EntityHandle::getScene() const{
    return scene;
}

Entity EntityHandle::getEntity() const{
    return entity;
}