// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Command.h"
#include <vector>
#include <cstddef>
#include <functional>

namespace doriax::editor{

    class CommandHistory{

    private:
        std::vector<Command*> list;
        size_t index = 0; // real index is (index-1)
        size_t sceneId = 0;

    public:
        // Called after any command apply/undo/redo with the scene id and affectsStructure().
        static std::function<void(size_t, bool)> onSceneModified;

        explicit CommandHistory(size_t sceneId = 0);
        virtual ~CommandHistory();

        void addCommand(Command* cmd);
        void addCommandNoMerge(Command* cmd);

        void undo();
        void redo();

        bool canUndo() const;
        bool canRedo() const;

        void clear();
    };

}

