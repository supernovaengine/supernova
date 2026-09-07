// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once

#include "Exporter.h"
#include "Generator.h"
#include "imgui.h"

#include <filesystem>
#include <string>
#include <vector>

namespace doriax::editor {

    class EditorSettingsWindow {
    private:
        bool m_isOpen = false;

        std::vector<CMakeKit> m_availableKits;
        int m_cmakeKitIndex = 0;
        std::string m_cmakeOverride;
        CMakeInfo m_cmakeInfo;
        std::string m_cmakePickError;
        int m_cmakeBuildJobs = 0;
        std::string m_cmakeBuildJobsTooltip;

        std::string m_emsdkOverride;
        EmsdkInfo m_emsdkInfo;
        bool m_editorVSyncEnabled = true;
        std::filesystem::path m_defaultExportDirectory;

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

        void open();
        void show();
        bool isOpen() const { return m_isOpen; }
    };

}
