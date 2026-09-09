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

    // Chooses which bundles with no scene instance the project builds and registers
    class BundlesWindow {
    private:
        struct BundleEntry {
            fs::path path;
            std::string usedBy;  // empty when no scene instantiates it
            bool included = false;
        };

        bool m_isOpen = false;
        Project* m_project = nullptr;
        std::vector<BundleEntry> m_entries;

        void drawBundles();
        void applyBundles();

    public:
        BundlesWindow() = default;
        ~BundlesWindow() = default;

        void open(Project* project);
        void show();
        bool isOpen() const { return m_isOpen; }
    };

}
