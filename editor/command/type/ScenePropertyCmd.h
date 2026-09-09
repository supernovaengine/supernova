// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "Scene.h"
#include "Catalog.h"

namespace doriax::editor {

    template <typename T>
    struct ScenePropertyCmdValue {
        T oldValue;
        T newValue;
    };

    template <typename T>
    class ScenePropertyCmd : public Command {

    private:
        // resolved via project->getScene(sceneId) at execute/undo time: Project::scenes is a
        // std::vector, so a stored SceneProject* would dangle after any scene open/create
        Project* project;
        uint32_t sceneId;
        std::string propertyName;
        ScenePropertyCmdValue<T> value;

        bool wasModified;

    public:
        ScenePropertyCmd(Project* project, uint32_t sceneId, const std::string& propertyName, const T& newValue) {
            this->project = project;
            this->sceneId = sceneId;
            this->propertyName = propertyName;
            this->value.newValue = newValue;

            SceneProject* sceneProject = project->getScene(sceneId);
            this->wasModified = sceneProject ? sceneProject->isModified : false;
        }

        bool execute() override {
            SceneProject* sceneProject = project->getScene(sceneId);
            if (!sceneProject) {
                return false;
            }

            // Store the old value before changing it
            value.oldValue = Catalog::getSceneProperty<T>(sceneProject->scene, propertyName);
            Catalog::setSceneProperty<T>(sceneProject->scene, propertyName, value.newValue);

            sceneProject->isModified = true;

            return true;
        }

        void undo() override {
            SceneProject* sceneProject = project->getScene(sceneId);
            if (!sceneProject) {
                return;
            }

            Catalog::setSceneProperty<T>(sceneProject->scene, propertyName, value.oldValue);

            sceneProject->isModified = wasModified;
        }

        // Render/physics settings only; scene names belong to SceneNameCmd.
        bool affectsStructure() const override { return false; }

        bool mergeWith(editor::Command* otherCommand) override {
            ScenePropertyCmd* otherCmd = dynamic_cast<ScenePropertyCmd*>(otherCommand);
            if (otherCmd != nullptr) {
                if (project == otherCmd->project && sceneId == otherCmd->sceneId && propertyName == otherCmd->propertyName) {
                    // Update oldValue from the earliest command
                    value.oldValue = otherCmd->value.oldValue;
                    value.newValue = otherCmd->value.newValue;

                    wasModified = wasModified && otherCmd->wasModified;

                    return true;
                }
            }
            return false;
        }
    };

}
