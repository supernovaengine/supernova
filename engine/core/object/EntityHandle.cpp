// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "EntityHandle.h"

#include <mutex>
#include <unordered_map>
#include <unordered_set>

using namespace doriax;

namespace {
    struct OwnedHandles{
        std::mutex mutex;
        std::unordered_map<Scene*, std::unordered_set<EntityHandle*>> handles;
    };

    // Never destroyed: a global scene, as in the project template, is destroyed after it
    OwnedHandles& ownedHandles(){
        static OwnedHandles* owned = new OwnedHandles();
        return *owned;
    }

    void trackOwned(Scene* scene, EntityHandle* handle){
        OwnedHandles& owned = ownedHandles();
        std::lock_guard<std::mutex> lock(owned.mutex);
        owned.handles[scene].insert(handle);
    }

    void untrackOwned(Scene* scene, EntityHandle* handle){
        OwnedHandles& owned = ownedHandles();
        std::lock_guard<std::mutex> lock(owned.mutex);
        owned.handles[scene].erase(handle);
    }
}

EntityHandle::EntityHandle(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    this->entityOwned = true;
    trackOwned(scene, this);
}

EntityHandle::EntityHandle(Scene* scene, Entity entity){
    this->scene = scene;
    this->entity = entity;
    this->entityOwned = false;
}

EntityHandle::~EntityHandle(){
    if (scene && entityOwned){
        untrackOwned(scene, this);
        // a script may have destroyed it already
        if (scene->isEntityCreated(entity))
            scene->destroyEntity(entity);
    }
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
        setEntityOwned(false);
        scene = rhs.scene;
        entity = rhs.entity;
    }

    return *this;
}

// Moves transfer ownership: the source is left as a non-owning observer so only the
// destination destroys the entity.
EntityHandle::EntityHandle(EntityHandle&& rhs) noexcept{
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = false;
    setEntityOwned(rhs.entityOwned);
    rhs.setEntityOwned(false);
}

EntityHandle& EntityHandle::operator=(EntityHandle&& rhs) noexcept{
    if (this != &rhs){
        setEntityOwned(false);
        scene = rhs.scene;
        entity = rhs.entity;
        setEntityOwned(rhs.entityOwned);
        rhs.setEntityOwned(false);
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
    if (scene && this->entityOwned != entityOwned){
        if (entityOwned)
            trackOwned(scene, this);
        else
            untrackOwned(scene, this);
    }
    this->entityOwned = entityOwned;
}

void EntityHandle::forgetScene(Scene* scene){
    OwnedHandles& owned = ownedHandles();
    std::lock_guard<std::mutex> lock(owned.mutex);
    auto it = owned.handles.find(scene);
    if (it == owned.handles.end())
        return;
    for (EntityHandle* handle : it->second){
        handle->entityOwned = false;
    }
    owned.handles.erase(it);
}
