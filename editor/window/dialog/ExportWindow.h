// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once

#include "Exporter.h"
#include "imgui.h"

#include <string>
#include <filesystem>
#include <functional>
#include <set>
#include <vector>

namespace doriax::editor {

    namespace fs = std::filesystem;

    class ExportWindow {
    private:
        // Modal flow: pick a mode card, configure it, watch the export run.
        enum class Step {
            ModeSelect,
            Settings,
            Progress
        };

        bool m_isOpen = false;
        Step m_step = Step::ModeSelect;
        ExportMode m_mode = ExportMode::SourceCode;
        Project* m_project = nullptr;

        // UI state
        char m_targetDirBuffer[512] = "";
        fs::path m_targetDir;
        bool m_targetDirFromDefault = false;

        // Start scene
        uint32_t m_startSceneId = NULL_PROJECT_SCENE;

        // Shader list: each entry is a shader to export
        struct ShaderEntry {
            ShaderKey key;
            ShaderType type;
            uint32_t properties;
            std::string displayName;
        };
        std::vector<ShaderEntry> m_shaderEntries;
        int m_selectedShaderIndex = -1;
        bool m_shaderKeysConfigured = false;

        // Add Shader dialog state
        bool m_addShaderOpen = false;
        int m_addShaderTypeIndex = 0;
        bool m_addShaderProps[32] = {};

        // Graphic backend selection (Source Code mode only)
        struct BackendEntry {
            ShaderBackend backend;
            std::string name;
            bool selected;
        };
        std::vector<BackendEntry> m_backendEntries;
        bool m_sourceBackendsConfigured = false;

        // Desktop backend selection. The entries themselves are host-specific
        // and live in ExportWindow.cpp.
        int m_graphicBackendIndex = 0;
        bool m_desktopBackendConfigured = false;

        // Cached tool checks: both spawn processes, so never probe per-frame.
        std::string m_emsdkOverride;           // mirrors AppSettings::getEmsdkPath()
        EmsdkInfo m_emsdkInfo;
        std::string m_missingBuildTools;       // Generator::checkBuildTools() result

        Exporter m_exporter;

        void populateShaderList();
        void populateShaderListFromKeys(const std::vector<ShaderKey>& shaderKeys);
        void loadShaderListFromSettings(const ExportTargetSettings& settings);
        void populateBackendList();
        void refreshEmsdkStatus();
        void selectMode(ExportMode mode);
        void loadSettingsFromProject();
        void saveCurrentSettingsToProject(bool saveProjectFile = true);
        void drawModeSelect();
        bool drawModeCard(const char* id, const char* icon, const char* title, const char* description,
                          const ImVec2& size, const char* disabledText = nullptr);
        void drawSettings();
        void drawOutputDirRow(const char* label);
        void drawStartSceneRow();
        void drawGraphicBackendRow();
        void drawDesktopKitRows();
        void drawEmsdkRow();
        void drawShaderSection();
        void drawBackendSection();
        void drawProgress();
        void drawAddShaderDialog();
        void drawOverwriteConfirmDialog();
        void startConfiguredExport(bool overwriteTarget);

    public:
        ExportWindow() = default;
        ~ExportWindow() = default;

        void open(Project* project);
        void show();
        bool isOpen() const { return m_isOpen; }
    };

}
