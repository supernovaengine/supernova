// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "Project.h"
#include <cstdint>
#include <vector>
#include <string>

#include "yaml-cpp/yaml.h"

namespace doriax::editor{

    class DuplicateEntityCmd: public Command{

    private:
        Project* project;
        uint32_t sceneId;

        std::vector<Entity> sourceEntities;
        std::vector<Entity> createdEntities;
        std::vector<Entity> lastSelected;
        bool wasModified;

    public:
        DuplicateEntityCmd(Project* project, uint32_t sceneId, const std::vector<Entity>& entities);

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;

        std::vector<Entity> getCreatedEntities() const;
    };

}
