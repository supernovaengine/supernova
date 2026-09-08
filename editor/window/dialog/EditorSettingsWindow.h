// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once

#include "AppSettings.h"
#include "Exporter.h"
#include "Generator.h"
#include "imgui.h"

#include <filesystem>
#include <string>
#include <vector>

namespace doriax::editor {

    class Project;

    class EditorSettingsWindow {
    private:
        bool m_isOpen = false;

        // Build settings are per project, so the dialog edits the open project's
        Project* m_project = nullptr;

        std::vector<CMakeKit> m_availableKits;
        int m_cmakeKitIndex = 0;
        std::string m_cmakeOverride;
        CMakeInfo m_cmakeInfo;
        std::string m_cmakePickError;

        std::string m_emsdkOverride;
        EmsdkInfo m_emsdkInfo;
        bool m_editorVSyncEnabled = true;
        std::filesystem::path m_defaultExportDirectory;

        LocalBuildSettings projectBuildSettings() const;
        void refreshCMakeStatus();
        void refreshEmsdkStatus();
        void drawSettings();
        void drawGeneralSettings();
        void drawCMakeSettings();
        void drawWebSettings();
        bool applySettings();

    public:
        EditorSettingsWindow() = default;
        ~EditorSettingsWindow() = default;

        void open(Project* project);
        void show();
        bool isOpen() const { return m_isOpen; }
    };

}
