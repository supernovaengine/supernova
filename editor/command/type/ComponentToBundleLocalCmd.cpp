// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ComponentToBundleLocalCmd.h"

#include "Stream.h"

using namespace doriax;

editor::ComponentToBundleLocalCmd::ComponentToBundleLocalCmd(Project* project, uint32_t sceneId, Entity entity, ComponentType componentType){
    this->project = project;
    this->sceneId = sceneId;
    this->entities.push_back(entity);
    this->componentType = componentType;

    this->wasModified = project->getScene(sceneId)->isModified;
}

bool editor::ComponentToBundleLocalCmd::execute() {
    SceneProject* sceneProject = project->getScene(sceneId);
    for (Entity entity : entities){
        fs::path filepath = project->findEntityBundlePathFor(sceneId, entity);
        if (sceneProject && !filepath.empty()) {
            EntityBundle* bundle = project->getEntityBundle(filepath);

            if (bundle->hasComponentOverride(sceneId, entity, componentType)){
                return false;
            }

            bundle->setComponentOverride(sceneProject->id, entity, componentType);
        }
    }

    sceneProject->isModified = true;

    return true;
}

void editor::ComponentToBundleLocalCmd::undo() {
    SceneProject* sceneProject = project->getScene(sceneId);
    for (Entity entity : entities){
        fs::path filepath = project->findEntityBundlePathFor(sceneId, entity);
        if (sceneProject && !filepath.empty()) {
            EntityBundle* bundle = project->getEntityBundle(filepath);

            bundle->clearComponentOverride(sceneProject->id, entity, componentType);

            Entity registryEntity = bundle->getRegistryEntity(sceneId, entity);
            Catalog::copyComponent(bundle->registry.get(), registryEntity, sceneProject->scene, entity, componentType);
            std::unordered_map<Entity, Entity> registryToLocal;
            if (const EntityBundle::Instance* instance = bundle->getInstance(sceneId, entity)) {
                for (const auto& member : instance->members) {
                    registryToLocal[member.registryEntity] = member.localEntity;
                }
            }
            Project::remapEntityPropertiesInComponent(sceneProject->scene, entity, componentType, {}, registryToLocal);
        }
    }

    sceneProject->isModified = wasModified;
}

bool editor::ComponentToBundleLocalCmd::mergeWith(Command* otherCommand){
    ComponentToBundleLocalCmd* otherCmd = dynamic_cast<ComponentToBundleLocalCmd*>(otherCommand);
    if (otherCmd != nullptr){
        if (sceneId == otherCmd->sceneId){

            for (Entity& otherEntity :  otherCmd->entities){
                entities.push_back(otherEntity);
            }

            wasModified = wasModified && otherCmd->wasModified;

            return true;
        }
    }

    return false;
}
