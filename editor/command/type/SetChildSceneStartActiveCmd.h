// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "Project.h"

namespace doriax::editor {
    class SetChildSceneStartActiveCmd : public Command {
    private:
        Project* project;
        uint32_t sceneId;
        uint32_t childSceneId;
        bool newStartActive;
        bool oldStartActive;
        bool wasModified;

    public:
        SetChildSceneStartActiveCmd(Project* project, uint32_t sceneId, uint32_t childSceneId, bool startActive);

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;
    };
}