// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "command/type/DeleteEntityCmd.h"
#include "command/type/ModelLoadCmd.h"
#include "Project.h"
#include "Catalog.h"
#include "component/ModelComponent.h"

#include <memory>

namespace doriax::editor {

    struct ComponentToBundleSharedData {
        Entity entity;
        YAML::Node recovery;
        // a model rebuilt from the shared file, the deleted tracked nodes of the old one,
        // the entities moved from them to the model and the settings of its runtime nodes
        bool modelRebuilt = false;
        std::shared_ptr<DeleteEntityCmd> modelNodesDeleteCmd;
        std::vector<ModelLoadCmd::ParkedEntity> modelAttachments;
        MeshSystem::MeshSettingsByNode modelSettings;
    };

    class ComponentToBundleSharedCmd: public Command {
    private:
        Project* project;
        uint32_t sceneId;
        ComponentType componentType;

        std::vector<ComponentToBundleSharedData> entities;

        bool wasModified;

        void rebuildModel(ComponentToBundleSharedData& entityData, const ModelComponent& shared);

    public:
        ComponentToBundleSharedCmd(Project* project, uint32_t sceneId, Entity entity, ComponentType componentType);

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;
    };

}
