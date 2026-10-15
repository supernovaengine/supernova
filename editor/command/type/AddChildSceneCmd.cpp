// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "AddChildSceneCmd.h"
#include "Out.h"

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

    return project->addChildScene(sceneId, childSceneId);
}

void editor::AddChildSceneCmd::undo() {
    project->removeChildScene(sceneId, childSceneId);
    project->getScene(sceneId)->isModified = wasModified;
}

// Another scene may have taken this one as a child since the undo
bool editor::AddChildSceneCmd::canRedo() const {
    if (!project->wouldCreateChildSceneCycle(sceneId, childSceneId)) {
        return true;
    }

    const char* name = project->getScene(sceneId)->name.c_str();
    const char* childName = project->getScene(childSceneId)->name.c_str();
    Out::error("Cannot add '%s' to '%s': '%s' already contains '%s'", childName, name, childName, name);
    return false;
}

bool editor::AddChildSceneCmd::mergeWith(Command* otherCommand){
    return false;
}
