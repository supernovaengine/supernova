// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ComponentToBundleSharedCmd.h"

#include "Stream.h"
#include "util/ProjectUtils.h"

using namespace doriax;

editor::ComponentToBundleSharedCmd::ComponentToBundleSharedCmd(Project* project, uint32_t sceneId, Entity entity, ComponentType componentType){
    this->project = project;
    this->sceneId = sceneId;
    this->componentType = componentType;

    ComponentToBundleSharedData entityData;
    entityData.entity = entity;
    entities.push_back(entityData);

    this->wasModified = project->getScene(sceneId)->isModified;
}

// The shared model builds its nodes when it loads, as in a fresh instance
void editor::ComponentToBundleSharedCmd::rebuildModel(ComponentToBundleSharedData& entityData, const ModelComponent& shared) {
    SceneProject* sceneProject = project->getScene(sceneId);
    Scene* scene = sceneProject->scene;
    const ModelComponent& model = scene->getComponent<ModelComponent>(entityData.entity);

    // Tracked nodes would be saved with the scene and doubled by the next load
    bool tracked = ProjectUtils::hasTrackedModelEntities(scene, model, sceneProject->entities);
    if (!tracked && model.filename == shared.filename && model.mergeStaticMeshes == shared.mergeStaticMeshes) {
        return;
    }

    // Entities hung on the old nodes move to the model, since the nodes the load builds are not saved
    entityData.modelAttachments = ModelLoadCmd::collectAttachments(sceneProject, entityData.entity, model);
    entityData.modelSettings = scene->getSystem<MeshSystem>()->collectMeshSettings(entityData.entity, model, model.filename);
    ModelLoadCmd::parkEntities(sceneProject, entityData.entity, entityData.modelAttachments);
    if (tracked) {
        entityData.modelNodesDeleteCmd = std::make_shared<DeleteEntityCmd>(project, sceneId, ModelLoadCmd::collectModelDeleteRoots(scene, model), true);
        entityData.modelNodesDeleteCmd->execute();
    }

    ModelComponent& rebuilt = scene->getComponent<ModelComponent>(entityData.entity);
    ProjectUtils::destroyModelNodes(scene, rebuilt);
    rebuilt.loadedFilename.clear();
    rebuilt.needUpdateModel = true;
    entityData.modelRebuilt = true;
}

bool editor::ComponentToBundleSharedCmd::execute() {
    SceneProject* sceneProject = project->getScene(sceneId);

    for (ComponentToBundleSharedData& entityData : entities){

        fs::path filepath = project->findEntityBundlePathFor(sceneId, entityData.entity);
        if (sceneProject && !filepath.empty()) {
            EntityBundle* bundle = project->getEntityBundle(filepath);

            Signature signature = Catalog::componentTypeToSignature(sceneProject->scene, componentType);
            entityData.recovery = Stream::encodeComponents(entityData.entity, sceneProject->scene, signature);

            if (!bundle->hasComponentOverride(sceneId, entityData.entity, componentType)){
                return false;
            }

            // Clear the override and copy values from the bundle registry
            bundle->clearComponentOverride(sceneProject->id, entityData.entity, componentType);

            Entity registryEntity = bundle->getRegistryEntity(sceneId, entityData.entity);
            if (componentType == ComponentType::ModelComponent) {
                rebuildModel(entityData, bundle->registry->getComponent<ModelComponent>(registryEntity));
            }
            Catalog::copyComponent(bundle->registry.get(), registryEntity, sceneProject->scene, entityData.entity, componentType);
            std::unordered_map<Entity, Entity> registryToLocal;
            if (const EntityBundle::Instance* instance = bundle->getInstance(sceneId, entityData.entity)) {
                for (const auto& member : instance->members) {
                    registryToLocal[member.registryEntity] = member.localEntity;
                }
            }
            Project::remapEntityPropertiesInComponent(sceneProject->scene, entityData.entity, componentType, {}, registryToLocal);
        }

    }

    sceneProject->isModified = true;

    return true;
}

void editor::ComponentToBundleSharedCmd::undo() {
    SceneProject* sceneProject = project->getScene(sceneId);

    for (ComponentToBundleSharedData& entityData : entities){

        fs::path filepath = project->findEntityBundlePathFor(sceneId, entityData.entity);
        if (sceneProject && !filepath.empty()) {
            EntityBundle* bundle = project->getEntityBundle(filepath);

            bundle->setComponentOverride(sceneProject->id, entityData.entity, componentType);

            Entity parent = NULL_ENTITY;
            if (componentType == ComponentType::Transform){
                parent = sceneProject->scene->getComponent<Transform>(entityData.entity).parent;
            }

            // The nodes of the shared file go, the old ones come back below
            if (entityData.modelRebuilt) {
                ProjectUtils::destroyModelNodes(sceneProject->scene, sceneProject->scene->getComponent<ModelComponent>(entityData.entity));
            }

            Stream::decodeComponents(entityData.entity, parent, sceneProject->scene, entityData.recovery);

            if (entityData.modelNodesDeleteCmd) {
                entityData.modelNodesDeleteCmd->undo();
                entityData.modelNodesDeleteCmd.reset();
                ModelLoadCmd::unparkEntities(sceneProject, entityData.modelAttachments);
            } else if (entityData.modelRebuilt && ModelLoadCmd::rebuildNodes(sceneProject->scene, entityData.entity, entityData.modelSettings)) {
                // Runtime nodes have no snapshot, the old file builds them again
                ModelLoadCmd::attachToNodes(sceneProject->scene, sceneProject->scene->getComponent<ModelComponent>(entityData.entity), entityData.modelAttachments);
                ProjectUtils::sortEntitiesByTransformOrder(sceneProject->scene, sceneProject->entities);
            }
            entityData.modelAttachments.clear();
            entityData.modelRebuilt = false;
        }

    }

    sceneProject->isModified = wasModified;
}

bool editor::ComponentToBundleSharedCmd::mergeWith(Command* otherCommand){
    ComponentToBundleSharedCmd* otherCmd = dynamic_cast<ComponentToBundleSharedCmd*>(otherCommand);
    if (otherCmd != nullptr){
        if (sceneId == otherCmd->sceneId){

            for (ComponentToBundleSharedData& otherEntityData :  otherCmd->entities){
                entities.push_back(otherEntityData);
            }

            wasModified = wasModified && otherCmd->wasModified;

            return true;
        }
    }

    return false;
}
