// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "command/type/CreateEntityCmd.h"
#include "command/type/PropertyCmd.h"
#include "component/AnimationComponent.h"

#include <memory>
#include <string>
#include <vector>

namespace doriax::editor {

    // Add a frame to an animation in one undo step, creating its action entity first
    // unless the type is EMPTY. The frame goes on the first free track from `track`.
    class AddActionFrameCmd : public Command {
    private:
        Project* project;
        uint32_t sceneId;
        Entity animEntity;
        uint32_t track;

        std::unique_ptr<CreateEntityCmd> createCmd;
        std::unique_ptr<PropertyCmd<std::vector<ActionFrame>>> actionsCmd;

    public:
        AddActionFrameCmd(Project* project, uint32_t sceneId, Entity animEntity, Entity target,
                          EntityCreationType creationType, std::string name, uint32_t track);

        bool execute() override;
        void undo() override;
        bool mergeWith(Command* otherCommand) override;
    };

}
