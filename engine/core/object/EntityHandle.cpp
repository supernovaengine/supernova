// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "EntityHandle.h"

using namespace doriax;

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

// Copies are non-owning observers: at most one handle may own an entity, otherwise the
// owned entity would be destroyed (and its sub-resources cleared) more than once. Use a
// move when you want to transfer ownership.
EntityHandle::EntityHandle(const EntityHandle& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = false;
}

EntityHandle& EntityHandle::operator=(const EntityHandle& rhs){
    if (this != &rhs){
        scene = rhs.scene;
        entity = rhs.entity;
        entityOwned = false;
    }

    return *this;
}

// Moves transfer ownership: the source is left as a non-owning observer so only the
// destination destroys the entity.
EntityHandle::EntityHandle(EntityHandle&& rhs) noexcept{
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;
    rhs.entityOwned = false;
}

EntityHandle& EntityHandle::operator=(EntityHandle&& rhs) noexcept{
    if (this != &rhs){
        scene = rhs.scene;
        entity = rhs.entity;
        entityOwned = rhs.entityOwned;
        rhs.entityOwned = false;
    }

    return *this;
}

Scene* EntityHandle::getScene() const{
    return scene;
}

Entity EntityHandle::getEntity() const{
    return entity;
}

void EntityHandle::setName(const std::string& name){
    scene->setEntityName(entity, name);
}

std::string EntityHandle::getName() const{
    return scene->getEntityName(entity);
}

bool EntityHandle::isEntityOwned() const{
    return entityOwned;
}

void EntityHandle::setEntityOwned(bool entityOwned){
    this->entityOwned = entityOwned;
}