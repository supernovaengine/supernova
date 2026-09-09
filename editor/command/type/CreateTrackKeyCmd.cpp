// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "CreateTrackKeyCmd.h"

#include "Catalog.h"
#include "component/KeyframeTracksComponent.h"
#include "component/RotateTracksComponent.h"
#include "component/ScaleTracksComponent.h"
#include "component/TranslateTracksComponent.h"

using namespace doriax;

namespace {

void seedCreateProperties(editor::CreateEntityCmd& createCmd, int channel, float localTime,
                          const Vector3& vectorValue, const Quaternion& quatValue){
    createCmd.addProperty<std::vector<float>>(
        editor::ComponentType::KeyframeTracksComponent, "times", std::vector<float>{localTime});
    if (channel == 0){
        createCmd.addProperty<std::vector<Vector3>>(
            editor::ComponentType::TranslateTracksComponent, "values", std::vector<Vector3>{vectorValue});
    }else if (channel == 1){
        createCmd.addProperty<std::vector<Quaternion>>(
            editor::ComponentType::RotateTracksComponent, "values", std::vector<Quaternion>{quatValue});
    }else{
        createCmd.addProperty<std::vector<Vector3>>(
            editor::ComponentType::ScaleTracksComponent, "values", std::vector<Vector3>{vectorValue});
    }
}

}

editor::CreateTrackKeyCmd::CreateTrackKeyCmd(Project* project, uint32_t sceneId, Entity animEntity, Entity target,
                                             EntityCreationType creationType, std::string name, float localTime,
                                             Vector3 value)
    : project(project), sceneId(sceneId), animEntity(animEntity), creationType(creationType),
      name(std::move(name)), localTime(localTime), channel(creationType == EntityCreationType::SCALE_TRACKS ? 2 : 0),
      vectorValue(value), quatValue(){
    createCmd = std::make_unique<CreateEntityCmd>(project, sceneId, this->name, creationType, target);
    seedCreateProperties(*createCmd, channel, localTime, vectorValue, quatValue);
    wasModified = project->getScene(sceneId)->isModified;
}

editor::CreateTrackKeyCmd::CreateTrackKeyCmd(Project* project, uint32_t sceneId, Entity animEntity, Entity target,
                                             EntityCreationType creationType, std::string name, float localTime,
                                             Quaternion value)
    : project(project), sceneId(sceneId), animEntity(animEntity), creationType(creationType),
      name(std::move(name)), localTime(localTime), channel(1), vectorValue(), quatValue(value){
    createCmd = std::make_unique<CreateEntityCmd>(project, sceneId, this->name, creationType, target);
    seedCreateProperties(*createCmd, channel, localTime, vectorValue, quatValue);
    wasModified = project->getScene(sceneId)->isModified;
}

bool editor::CreateTrackKeyCmd::execute(){
    SceneProject* sceneProject = project->getScene(sceneId);
    if (!sceneProject || !sceneProject->scene || !createCmd){
        return false;
    }
    Scene* scene = sceneProject->scene;
    if (!scene->findComponent<AnimationComponent>(animEntity)){
        return false;
    }

    if (!createCmd->execute()){
        return false;
    }
    trackEntity = createCmd->getEntity();
    if (trackEntity == NULL_ENTITY){
        return false;
    }

    AnimationComponent& anim = scene->getComponent<AnimationComponent>(animEntity);
    oldActions = anim.actions;
    std::vector<ActionFrame> newActions = oldActions;
    // Duration 0 = auto: the block grows as keys extend.
    newActions.push_back({0.0f, 0.0f, trackEntity, 0});

    PropertyData prop = Catalog::getProperty(scene, animEntity, ComponentType::AnimationComponent, "actions");
    auto* actionsRef = static_cast<std::vector<ActionFrame>*>(prop.ref);
    if (!actionsRef){
        createCmd->undo();
        trackEntity = NULL_ENTITY;
        return false;
    }
    *actionsRef = newActions;
    Catalog::updateEntity(scene, animEntity, prop.updateFlags);
    if (project->isEntityInBundle(sceneId, animEntity)){
        project->bundlePropertyChanged(sceneId, animEntity, ComponentType::AnimationComponent, {"actions"});
    }

    sceneProject->isModified = true;
    return true;
}

void editor::CreateTrackKeyCmd::undo(){
    SceneProject* sceneProject = project->getScene(sceneId);
    if (!sceneProject || !sceneProject->scene){
        return;
    }
    Scene* scene = sceneProject->scene;

    if (AnimationComponent* anim = scene->findComponent<AnimationComponent>(animEntity)){
        PropertyData prop = Catalog::getProperty(scene, animEntity, ComponentType::AnimationComponent, "actions");
        if (auto* actionsRef = static_cast<std::vector<ActionFrame>*>(prop.ref)){
            *actionsRef = oldActions;
            Catalog::updateEntity(scene, animEntity, prop.updateFlags);
            if (project->isEntityInBundle(sceneId, animEntity)){
                project->bundlePropertyChanged(sceneId, animEntity, ComponentType::AnimationComponent, {"actions"});
            }
        }else{
            anim->actions = oldActions;
        }
    }

    if (createCmd){
        createCmd->undo();
    }
    trackEntity = NULL_ENTITY;
    sceneProject->isModified = wasModified;
}

bool editor::CreateTrackKeyCmd::mergeWith(Command* otherCommand){
    (void)otherCommand;
    return false;
}
