// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "AddChildSceneCmd.h"

using namespace doriax;

editor::AddChildSceneCmd::AddChildSceneCmd(Project* project, uint32_t sceneId, uint32_t childSceneId){
    this->project = project;
    this->sceneId = sceneId;
    this->childSceneId = childSceneId;

    this->wasModified = project->getScene(sceneId)->isModified;
}

bool editor::AddChildSceneCmd::execute() {
    if (project->hasChildScene(sceneId, childSceneId)) {
        return false;
    }

    project->addChildScene(sceneId, childSceneId);
    return true;
}

void editor::AddChildSceneCmd::undo() {
    project->removeChildScene(sceneId, childSceneId);
    project->getScene(sceneId)->isModified = wasModified;
}

bool editor::AddChildSceneCmd::mergeWith(Command* otherCommand){
    return false;
}
