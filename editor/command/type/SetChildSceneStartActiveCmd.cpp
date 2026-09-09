// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "SetChildSceneStartActiveCmd.h"

using namespace doriax;

editor::SetChildSceneStartActiveCmd::SetChildSceneStartActiveCmd(Project* project, uint32_t sceneId, uint32_t childSceneId, bool startActive) {
    this->project = project;
    this->sceneId = sceneId;
    this->childSceneId = childSceneId;
    this->newStartActive = startActive;
    this->oldStartActive = project->isChildSceneStartActive(sceneId, childSceneId);

    SceneProject* sceneProject = project->getScene(sceneId);
    this->wasModified = sceneProject ? sceneProject->isModified : false;
}

bool editor::SetChildSceneStartActiveCmd::execute() {
    if (!project->hasChildScene(sceneId, childSceneId)) {
        return false;
    }

    oldStartActive = project->isChildSceneStartActive(sceneId, childSceneId);
    if (oldStartActive == newStartActive) {
        return false;
    }

    project->setChildSceneStartActive(sceneId, childSceneId, newStartActive);
    return true;
}

void editor::SetChildSceneStartActiveCmd::undo() {
    project->setChildSceneStartActive(sceneId, childSceneId, oldStartActive);

    SceneProject* sceneProject = project->getScene(sceneId);
    if (sceneProject) {
        sceneProject->isModified = wasModified;
    }
}

bool editor::SetChildSceneStartActiveCmd::mergeWith(Command* otherCommand) {
    return false;
}