// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Project.h"
#include "imgui.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace doriax::editor {

class ShaderForkDialog {
private:
    // What the dialog draws. Planning copies every include the fork would create, so
    // it reruns only when one of the inputs it depends on changes.
    struct ForkPreview {
        bool valid = false;
        std::string error;
        std::string vertPath;                    // all paths project-relative
        std::string fragPath;
        std::string forkDir;                     // empty unless includes are forked
        std::vector<std::string> includeFiles;
    };

    bool m_isOpen = false;
    Project* m_project = nullptr;
    ShaderType m_shaderType = ShaderType::MESH;
    std::filesystem::path m_projectPath;
    std::string m_selectedPath;
    char m_nameBuffer[128] = {};
    bool m_forkIncludes = false;
    std::function<void(const std::filesystem::path&, const std::string&, bool)> m_onCreate;
    std::function<void()> m_onCancel;

    ForkPreview m_preview;
    std::filesystem::path m_previewDirectory;
    std::string m_previewName;
    bool m_previewForkIncludes = false;
    bool m_previewDirty = true;

    static int shaderNameCharFilter(ImGuiInputTextCallbackData* data);
    static const char* shaderTypeName(ShaderType shaderType);
    const ForkPreview& refreshPreview(const std::filesystem::path& directory, const std::string& name);

public:
    void open(Project* project, ShaderType shaderType, const std::string& defaultBaseName,
              std::function<void(const std::filesystem::path&, const std::string&, bool)> onCreate,
              std::function<void()> onCancel = nullptr);

    void show();
    bool isOpen() const { return m_isOpen; }
    void close() { m_isOpen = false; }
};

} // namespace doriax::editor
