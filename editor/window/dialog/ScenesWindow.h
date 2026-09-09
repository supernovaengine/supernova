// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Project.h"
#include "imgui.h"

#include <string>
#include <filesystem>
#include <vector>

namespace doriax::editor {

    namespace fs = std::filesystem;

    // Chooses which scene files found in the project folder are part of the project
    class ScenesWindow {
    private:
        struct ParentRef {
            uint32_t sceneId = NULL_PROJECT_SCENE;
            std::string name;
        };

        struct SceneEntry {
            fs::path path;
            uint32_t sceneId = NULL_PROJECT_SCENE;
            std::vector<ParentRef> parents;  // scenes holding it as a child scene
            bool opened = false;
            bool included = false;
            bool wasIncluded = false;
        };

        bool m_isOpen = false;
        Project* m_project = nullptr;
        std::vector<SceneEntry> m_entries;

        void drawScenes();
        void applyScenes();

        bool isParentKept(uint32_t parentSceneId) const;
        std::string keptParentNames(const SceneEntry& entry) const;

    public:
        ScenesWindow() = default;
        ~ScenesWindow() = default;

        void open(Project* project);
        void show();
        bool isOpen() const { return m_isOpen; }
    };

}
