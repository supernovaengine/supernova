// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "RemoveComponentCmd.h"

#include "Out.h"
#include "command/type/DeleteEntityCmd.h"
#include "util/CameraTextureLink.h"
#include "util/ProjectUtils.h"

#include <algorithm>
#include <map>
#include <set>
#include <utility>

using namespace doriax;

namespace {

void appendUnique(std::vector<Entity>& entities, Entity entity) {
    if (std::find(entities.begin(), entities.end(), entity) == entities.end()) {
        entities.push_back(entity);
    }
}

bool hasOwnedComponent(Scene* scene, Entity entity, editor::ComponentType componentType) {
    if (!scene || !scene->isEntityCreated(entity)) {
        return false;
    }
    if (componentType == editor::ComponentType::ActionComponent) {
        return scene->findComponent<ActionComponent>(entity) != nullptr;
    }
    if (componentType == editor::ComponentType::AnimationComponent) {
        return scene->findComponent<AnimationComponent>(entity) != nullptr;
    }
    return false;
}

}

editor::RemoveComponentCmd::RemoveComponentCmd(Project* project, size_t sceneId, Entity entity, ComponentType componentType){
    this->project = project;
    this->sceneId = sceneId;
    this->componentType = componentType;

    RemoveComponentData entityData;
    entityData.entity = entity;

    this->entities.push_back(entityData);

    this->wasModified = project->getScene(sceneId)->isModified;
}

editor::RemoveComponentCmd::~RemoveComponentCmd() = default;

bool editor::RemoveComponentCmd::execute() {
    SceneProject* sceneProject = project->getScene(sceneId);
    if (sceneProject) {
        Scene* scene = sceneProject->scene;

        if (!ownedEntitiesCaptured) {
            ownedEntitiesCaptured = true;

            std::map<uint32_t, std::vector<Entity>> ownersByScene;
            auto addOwner = [&](uint32_t ownerSceneId, Entity owner) {
                SceneProject* ownerScene = project->getScene(ownerSceneId);
                if (ownerScene && hasOwnedComponent(ownerScene->scene, owner, componentType)) {
                    appendUnique(ownersByScene[ownerSceneId], owner);
                }
            };

            for (const RemoveComponentData& entityData : entities) {
                fs::path bundlePath = project->findEntityBundlePathFor(sceneId, entityData.entity);
                EntityBundle* bundle = project->getEntityBundle(bundlePath);
                bool sharedComponent = bundle &&
                    !bundle->hasComponentOverride(sceneId, entityData.entity, componentType);
                if (!sharedComponent) {
                    addOwner(sceneId, entityData.entity);
                    continue;
                }

                Entity registryEntity = bundle->getRegistryEntity(sceneId, entityData.entity);
                if (registryEntity == NULL_ENTITY) {
                    continue;
                }
                for (const auto& [otherSceneId, instances] : bundle->instances) {
                    SceneProject* otherScene = project->getScene(otherSceneId);
                    if (!otherScene || !otherScene->scene) {
                        continue;
                    }
                    for (const EntityBundle::Instance& instance : instances) {
                        Entity otherOwner = bundle->getLocalEntity(
                            otherSceneId, instance.instanceId, registryEntity);
                        if (otherOwner != NULL_ENTITY &&
                            !bundle->hasComponentOverride(otherSceneId, otherOwner, componentType)) {
                            addOwner(otherSceneId, otherOwner);
                        }
                    }
                }
            }

            std::vector<uint32_t> orderedSceneIds;
            if (ownersByScene.count(sceneId)) {
                orderedSceneIds.push_back(sceneId);
            }
            for (const auto& [ownerSceneId, owners] : ownersByScene) {
                if (ownerSceneId != sceneId) {
                    orderedSceneIds.push_back(ownerSceneId);
                }
            }

            std::map<uint32_t, std::vector<Entity>> targetsByScene;
            std::set<std::pair<fs::path, Entity>> sharedTargets;
            std::set<std::pair<uint32_t, Entity>> localTargets;

            for (uint32_t ownerSceneId : orderedSceneIds) {
                SceneProject* ownerScene = project->getScene(ownerSceneId);
                const std::vector<Entity>& preservedOwners = ownersByScene.at(ownerSceneId);
                for (Entity owner : preservedOwners) {
                    for (Entity target : ProjectUtils::getOwnedCascadeEntities(
                            ownerScene->scene, owner, componentType)) {
                        if (!ownerScene->scene->isEntityCreated(target)) {
                            continue;
                        }
                        if (std::find(preservedOwners.begin(), preservedOwners.end(), target) != preservedOwners.end()) {
                            continue;
                        }

                        fs::path targetBundlePath = project->findEntityBundlePathFor(ownerSceneId, target);
                        EntityBundle* targetBundle = project->getEntityBundle(targetBundlePath);
                        Entity registryTarget = targetBundle ?
                            targetBundle->getRegistryEntity(ownerSceneId, target) : NULL_ENTITY;
                        if (registryTarget != NULL_ENTITY) {
                            if (sharedTargets.insert({targetBundlePath, registryTarget}).second) {
                                targetsByScene[ownerSceneId].push_back(target);
                            }
                        } else if (localTargets.insert({ownerSceneId, target}).second) {
                            targetsByScene[ownerSceneId].push_back(target);
                        }
                    }
                }
            }

            for (uint32_t ownerSceneId : orderedSceneIds) {
                auto targets = targetsByScene.find(ownerSceneId);
                if (targets != targetsByScene.end() && !targets->second.empty()) {
                    ownedEntityDeletes.emplace_back(new DeleteEntityCmd(project,
                        ownerSceneId, targets->second, true, ownersByScene.at(ownerSceneId)));
                }
            }
        }

        for (const std::unique_ptr<DeleteEntityCmd>& ownedDelete : ownedEntityDeletes) {
            ownedDelete->execute();
        }

        for (RemoveComponentData& entityData : entities){

            fs::path bundlePath = project->findEntityBundlePathFor(sceneId, entityData.entity);
            EntityBundle* bundle = project->getEntityBundle(bundlePath);

            if (bundle && componentType == ComponentType::Transform){
                Out::error("Cannot remove Transform component from bundle entity '%s' in scene '%s'", scene->getEntityName(entityData.entity).c_str(), sceneProject->name.c_str());
                continue;
            }

            if (bundle && !bundle->hasComponentOverride(sceneId, entityData.entity, componentType)){

                entityData.recovery = project->removeComponentFromBundle(sceneId, entityData.entity, componentType);

            }else{

                entityData.oldComponent = ProjectUtils::removeEntityComponent(scene, entityData.entity, componentType, sceneProject->entities, true);

                if (bundle){
                    entityData.hasOverride = bundle->hasComponentOverride(sceneId, entityData.entity, componentType);
                    if (entityData.hasOverride){
                        bundle->clearComponentOverride(sceneId, entityData.entity, componentType);
                    }
                }

            }
        }

        ProjectUtils::reconcileTrackedEntities(scene, sceneProject->entities, project, sceneId);

        if (componentType == ComponentType::CameraComponent){
            CameraTextureLink::resolve(sceneProject->scene);
        }

        sceneProject->isModified = true;
    }

    return true;
}

void editor::RemoveComponentCmd::undo() {
    SceneProject* sceneProject = project->getScene(sceneId);
    if (sceneProject) {
        Scene* scene = sceneProject->scene;

        for (RemoveComponentData& entityData : entities){
            if (entityData.recovery.size() > 0){

                project->addComponentToBundle(sceneId, entityData.entity, componentType, entityData.recovery, true);

            }else{

                ProjectUtils::addEntityComponent(scene, entityData.entity, componentType, sceneProject->entities, entityData.oldComponent);

                if (entityData.hasOverride){
                    fs::path bundlePath = project->findEntityBundlePathFor(sceneId, entityData.entity);
                    EntityBundle* bundle = project->getEntityBundle(bundlePath);

                    if (bundle){
                        bundle->setComponentOverride(sceneId, entityData.entity, componentType);
                    }
                }

            }
        }

        // Restore owned entities after their owner components are back. DeleteEntityCmd then
        // also repairs those components if an occupied ID forced a replacement allocation.
        for (auto ownedDelete = ownedEntityDeletes.rbegin();
            ownedDelete != ownedEntityDeletes.rend(); ++ownedDelete) {
            (*ownedDelete)->undo();
        }

        if (componentType == ComponentType::CameraComponent){
            CameraTextureLink::resolve(sceneProject->scene);
        }

        sceneProject->isModified = wasModified;
    }
}

bool editor::RemoveComponentCmd::mergeWith(Command* otherCommand){
    RemoveComponentCmd* otherCmd = dynamic_cast<RemoveComponentCmd*>(otherCommand);
    if (otherCmd != nullptr){
        if (project == otherCmd->project && sceneId == otherCmd->sceneId &&
            componentType == otherCmd->componentType){

            std::vector<std::unique_ptr<DeleteEntityCmd>> mergedDeletes;
            mergedDeletes.reserve(otherCmd->ownedEntityDeletes.size() + ownedEntityDeletes.size());
            for (std::unique_ptr<DeleteEntityCmd>& ownedDelete : otherCmd->ownedEntityDeletes) {
                mergedDeletes.push_back(std::move(ownedDelete));
            }
            for (std::unique_ptr<DeleteEntityCmd>& ownedDelete : ownedEntityDeletes) {
                mergedDeletes.push_back(std::move(ownedDelete));
            }
            ownedEntityDeletes = std::move(mergedDeletes);
            ownedEntitiesCaptured = ownedEntitiesCaptured && otherCmd->ownedEntitiesCaptured;

            for (RemoveComponentData& otherEntityData :  otherCmd->entities){
                entities.push_back(otherEntityData);
            }

            wasModified = wasModified && otherCmd->wasModified;

            return true;
        }
    }

    return false;
}
