// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "imgui.h"
#include "texture/Texture.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace doriax::editor {

    class Project;

    class ImageViewerWindow {
    private:
        static constexpr float MIN_ZOOM = 0.01f;
        static constexpr float MAX_ZOOM = 32.0f;

        struct Instance {
            bool isOpen = true;
            bool focusRequested = false;
            bool fitToWindow = true;

            std::filesystem::path filepath;
            std::string loadError;
            Texture texture;

            float zoom = 1.0f;
            float lastDisplayScale = 1.0f;
            ImVec2 pan = ImVec2(0.0f, 0.0f);
        };

        Project* project;
        std::unordered_map<std::string, Instance> instances;
        std::vector<Texture> retiredTextures;
        uint64_t textureGeneration = 0;

        std::filesystem::path resolveFilepath(const std::filesystem::path& path) const;
        std::string toRelativePath(const std::filesystem::path& path) const;
        void loadImage(Instance& instance);
        void drawImage(Instance& instance);
        void retireTexture(Texture& texture);
        void releaseRetiredTextures();

    public:
        explicit ImageViewerWindow(Project* project);
        ~ImageViewerWindow();

        void openFile(const std::filesystem::path& path, bool dockToCentral = true);
        void closeFile(const std::filesystem::path& path);
        void closeAll();
        void show();

        bool isFileOpen(const std::filesystem::path& path) const;
        bool handleFileRename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

        static std::string getWindowId(const std::filesystem::path& relativePath);
    };

}
