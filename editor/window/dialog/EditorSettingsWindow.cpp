// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#include "EditorSettingsWindow.h"

#include "AppSettings.h"
#include "Project.h"
#include "Theme.h"
#include "external/IconsFontAwesome6.h"
#include "util/FileDialogs.h"
#include "window/Widgets.h"

#include <algorithm>
#include <filesystem>
#include <functional>

namespace doriax::editor {

namespace {
    namespace fs = std::filesystem;

    static constexpr float dialogWidth = 700.0f;
    static constexpr float dialogHeight = 360.0f;
    static constexpr float settingsLabelWidth = 190.0f;
    static constexpr float settingsPanelPadding = 12.0f;
    static constexpr float settingsButtonWidth = 120.0f;
    static constexpr ImGuiWindowFlags noScrollFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    template <typename DrawContents>
    void drawSettingsPanel(const char* panelId, DrawContents drawContents) {
        const ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Theme::dpi(ImVec2(settingsPanelPadding, settingsPanelPadding)));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(style.CellPadding.x, Theme::dpi(6.0f)));

        bool panelVisible = ImGui::BeginChild(panelId, ImVec2(0, 0), ImGuiChildFlags_Borders);
        float labelWidth = std::min(Theme::dpi(settingsLabelWidth), ImGui::GetContentRegionAvail().x * 0.4f);
        if (panelVisible && ImGui::BeginTable("##EditorSettingsTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            drawContents();
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
    }

    bool beginSettingsRow(const char* label, const char* tooltip = nullptr, bool defChanged = false) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        if (tooltip) ImGui::SetItemTooltip("%s", tooltip);

        bool reset = false;
        if (defChanged) {
            const ImGuiStyle& style = ImGui::GetStyle();
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, style.ItemSpacing.y));
            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, style.FramePadding.y));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
            reset = ImGui::Button((ICON_FA_ROTATE_LEFT "##" + std::string(label)).c_str());
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(3);
            ImGui::SetItemTooltip("Restore default");
        }

        ImGui::TableNextColumn();
        return reset;
    }

    void showDisabledItemTooltip(const std::string& text) {
        if (!ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) return;

        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 25.0f);
        ImGui::TextUnformatted(text.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    float buttonVisibleWidth(const char* label) {
        return ImGui::CalcTextSize(label, nullptr, true).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    }

    void drawPathPicker(
        const char* id,
        const char* browseId,
        const char* autoId,
        fs::path displayPath,
        bool canAuto,
        bool selectDirectory,
        const std::function<void(const std::string&)>& onBrowse,
        const std::function<void()>& onAuto
    ) {
        const ImGuiStyle& style = ImGui::GetStyle();
        float browseWidth = buttonVisibleWidth(browseId);
        float autoWidth = buttonVisibleWidth(autoId);
        float pathWidth = std::max(1.0f, ImGui::GetContentRegionAvail().x - browseWidth - autoWidth - style.ItemSpacing.x * 2.0f);

        Widgets::pathDisplay(id, displayPath, Vector2(pathWidth, ImGui::GetFrameHeight()));

        ImGui::SameLine();
        if (ImGui::Button(browseId)) {
            std::string startDir;
            if (!displayPath.empty() && displayPath.string().rfind("<", 0) != 0) {
                startDir = displayPath.parent_path().string();
            }
            std::string selectedPath = FileDialogs::openFileDialog(startDir, FILE_DIALOG_ALL, selectDirectory);
            if (!selectedPath.empty()) {
                onBrowse(selectedPath);
            }
        }

        ImGui::SameLine();
        ImGui::BeginDisabled(!canAuto);
        if (ImGui::Button(autoId)) {
            onAuto();
        }
        ImGui::EndDisabled();
        ImGui::SetItemTooltip("Clear the override and use auto-detect again.");
    }
}

void EditorSettingsWindow::open(Project* project) {
    m_isOpen = true;
    m_project = project;

    m_availableKits = Generator::detectAvailableKits();
    m_cmakeKitIndex = 0;
    m_cmakeOverride = AppSettings::getCMakePath();
    m_cmakePickError.clear();
    refreshCMakeStatus();

    const LocalBuildSettings currentBuild = projectBuildSettings();
    const std::string& currentCxx = currentBuild.cxxCompiler;
    const std::string& currentGen = currentBuild.generator;
    if (!currentCxx.empty() || !currentGen.empty()) {
        for (size_t i = 0; i < m_availableKits.size(); i++) {
            if (m_availableKits[i].available && m_availableKits[i].cxxCompiler == currentCxx && m_availableKits[i].generator == currentGen) {
                m_cmakeKitIndex = static_cast<int>(i + 1);
                break;
            }
        }
    }

    m_emsdkOverride = AppSettings::getEmsdkPath();
    m_editorVSyncEnabled = AppSettings::getEditorVSyncEnabled();
    m_defaultExportDirectory = AppSettings::getDefaultExportDirectory();
    refreshEmsdkStatus();
}

LocalBuildSettings EditorSettingsWindow::projectBuildSettings() const {
    if (!m_project) return LocalBuildSettings();
    return AppSettings::getBuildSettings(m_project->getProjectPath() / "project.yaml");
}

void EditorSettingsWindow::refreshCMakeStatus() {
    m_cmakeInfo = Generator::detectCMake();
}

void EditorSettingsWindow::refreshEmsdkStatus() {
    m_emsdkInfo = Exporter::detectEmsdk(m_emsdkOverride);
}

void EditorSettingsWindow::show() {
    if (!m_isOpen) return;

    ImGui::OpenPopup("Editor Settings##EditorSettingsModal");

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 center = viewport->GetWorkCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size(
        std::min(Theme::dpi(dialogWidth), viewport->WorkSize.x * 0.9f),
        std::min(Theme::dpi(dialogHeight), viewport->WorkSize.y * 0.9f)
    );
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_Modal |
                             ImGuiWindowFlags_NoResize |
                             noScrollFlags;

    bool popupOpen = ImGui::BeginPopupModal("Editor Settings##EditorSettingsModal", &m_isOpen, flags);

    if (popupOpen) {
        if (!m_isOpen) {
            ImGui::CloseCurrentPopup();
        } else {
            drawSettings();
        }
        ImGui::EndPopup();
    }
}

void EditorSettingsWindow::drawSettings() {
    const ImGuiStyle& style = ImGui::GetStyle();
    const float footerY = ImGui::GetWindowHeight() - style.WindowPadding.y - ImGui::GetFrameHeight();
    float tabRegionHeight = std::max(1.0f, footerY - style.ItemSpacing.y - ImGui::GetCursorPosY());

    ImGui::BeginChild(
        "##EditorSettingsTabRegion",
        ImVec2(0, tabRegionHeight),
        ImGuiChildFlags_None,
        noScrollFlags
    );

    if (ImGui::BeginTabBar("##EditorSettingsTabs", ImGuiTabBarFlags_FittingPolicyShrink)) {
        if (ImGui::BeginTabItem("General")) {
            drawGeneralSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Desktop")) {
            drawCMakeSettings();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Web")) {
            drawWebSettings();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(style.WindowPadding.x, footerY - style.ItemSpacing.y));
    ImGui::Separator();

    float footerWidth = std::max(1.0f, ImGui::GetWindowWidth() - style.WindowPadding.x * 2.0f);
    float buttonWidth = std::clamp((footerWidth - style.ItemSpacing.x) * 0.5f, 1.0f, Theme::dpi(settingsButtonWidth));
    float buttonsWidth = buttonWidth * 2.0f + style.ItemSpacing.x;
    float buttonX = style.WindowPadding.x + std::max(0.0f, (footerWidth - buttonsWidth) * 0.5f);
    ImGui::SetCursorPos(ImVec2(buttonX, footerY));

    if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
        if (applySettings()) {
            m_isOpen = false;
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }
}

void EditorSettingsWindow::drawGeneralSettings() {
    drawSettingsPanel("##EditorGeneralSettingsPanel", [this]() {
        if (beginSettingsRow("Default Export Directory",
                             "Used by Export Project when the current project has no saved export directory.",
                             !m_defaultExportDirectory.empty())) {
            m_defaultExportDirectory.clear();
        }
        fs::path exportDisplay = m_defaultExportDirectory.empty()
            ? fs::path("<Not set>")
            : m_defaultExportDirectory;
        drawPathPicker(
            "##EditorDefaultExportDirectory", "Browse##editor_default_export_dir", "Clear##editor_default_export_dir",
            exportDisplay, !m_defaultExportDirectory.empty(), true,
            [this](const std::string& selectedPath) {
                m_defaultExportDirectory = selectedPath;
            },
            [this]() {
                m_defaultExportDirectory.clear();
            }
        );

        if (beginSettingsRow("Editor VSync",
                             "VSync for the editor UI. Play mode and exported builds use the project VSync setting.",
                             m_editorVSyncEnabled != true)) {
            m_editorVSyncEnabled = true;
        }
        ImGui::Checkbox("##EditorVSync", &m_editorVSyncEnabled);
    });
}

void EditorSettingsWindow::drawCMakeSettings() {
    drawSettingsPanel("##EditorCMakeSettingsPanel", [this]() {
        beginSettingsRow("CMake", "Path to cmake executable. Empty means auto-detect from PATH.", !m_cmakeOverride.empty());
        fs::path cmakeDisplay = m_cmakeOverride.empty()
            ? fs::path(m_cmakeInfo.found ? m_cmakeInfo.path : "<Not found on PATH>")
            : fs::path(m_cmakeOverride);
        drawPathPicker(
            "##EditorCMakePath", "Browse##editor_cmake", "Auto##editor_cmake",
            cmakeDisplay, !m_cmakeOverride.empty(), false,
            [this](const std::string& selectedPath) {
                const std::string resolved = Generator::resolveCMakePath(selectedPath);
                const std::string version = resolved.empty() ? std::string() : Generator::probeCMakeVersion(resolved);

                if (version.empty()) {
                    m_cmakePickError = resolved.empty()
                        ? "No CMake executable in: " + selectedPath
                        : "Not a working CMake: " + resolved;
                } else {
                    m_cmakeOverride = resolved;
                    m_cmakePickError.clear();
                }
                refreshCMakeStatus();
            },
            [this]() {
                m_cmakeOverride.clear();
                m_cmakePickError.clear();
                refreshCMakeStatus();
            }
        );

        if (m_cmakeInfo.found) {
            ImGui::TextDisabled("Detected: %s", m_cmakeInfo.version.empty() ? m_cmakeInfo.path.c_str() : m_cmakeInfo.version.c_str());
        }
        if (!m_cmakePickError.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            ImGui::TextWrapped("%s", m_cmakePickError.c_str());
            ImGui::PopStyleColor();
        }

        if (beginSettingsRow("Compiler", "Compiler kit used to build C++ scripts, both when playing a scene and when exporting.", m_cmakeKitIndex != 0)) {
            m_cmakeKitIndex = 0;
        }
        if (m_cmakeKitIndex < 0 || m_cmakeKitIndex > static_cast<int>(m_availableKits.size())) {
            m_cmakeKitIndex = 0;
        }

        const char* currentLabel = m_cmakeKitIndex == 0
            ? "Default"
            : m_availableKits[m_cmakeKitIndex - 1].displayName.c_str();

        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##EditorCMakeKit", currentLabel)) {
            bool selected = m_cmakeKitIndex == 0;
            if (ImGui::Selectable("Default", selected)) m_cmakeKitIndex = 0;
            if (selected) ImGui::SetItemDefaultFocus();

            for (size_t i = 0; i < m_availableKits.size(); i++) {
                const auto& kit = m_availableKits[i];
                if (!kit.available) {
                    ImGui::BeginDisabled();
                    ImGui::Selectable((kit.displayName + "  (unavailable)").c_str(), false);
                    ImGui::EndDisabled();
                    showDisabledItemTooltip(kit.unavailableReason);
                    continue;
                }

                selected = m_cmakeKitIndex == static_cast<int>(i + 1);
                if (ImGui::Selectable(kit.displayName.c_str(), selected)) {
                    m_cmakeKitIndex = static_cast<int>(i + 1);
                }
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (m_cmakeKitIndex > 0) {
            const auto& kit = m_availableKits[m_cmakeKitIndex - 1];
            if (!kit.cCompiler.empty() || !kit.cxxCompiler.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                ImGui::TextWrapped("C: %s\nCXX: %s", kit.cCompiler.c_str(), kit.cxxCompiler.c_str());
                ImGui::PopStyleColor();
            }
        } else {
            // The combo fell back to "Default" because the stored kit was not detected
            const LocalBuildSettings stored = projectBuildSettings();
            if (!stored.cxxCompiler.empty() || !stored.generator.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
                ImGui::TextWrapped(ICON_FA_TRIANGLE_EXCLAMATION " Builds still use \"%s\", which is no longer detected. Apply to switch to the default toolchain.",
                    stored.cxxCompiler.empty() ? stored.generator.c_str() : stored.cxxCompiler.c_str());
                ImGui::PopStyleColor();
            }
        }

    });
}

void EditorSettingsWindow::drawWebSettings() {
    drawSettingsPanel("##EditorWebSettingsPanel", [this]() {
        beginSettingsRow("Emscripten SDK", "Path to emsdk root. Empty means EMSDK/PATH auto-detect.", !m_emsdkOverride.empty());

        fs::path emsdkDisplay = m_emsdkOverride.empty()
            ? fs::path("<Auto-detect>")
            : fs::path(m_emsdkOverride);
        drawPathPicker(
            "##EditorEmsdkPath", "Browse##editor_emsdk", "Auto##editor_emsdk",
            emsdkDisplay, !m_emsdkOverride.empty(), true,
            [this](const std::string& selectedPath) {
                m_emsdkOverride = selectedPath;
                refreshEmsdkStatus();
            },
            [this]() {
                m_emsdkOverride.clear();
                refreshEmsdkStatus();
            }
        );

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Status");
        ImGui::TableNextColumn();
        if (m_emsdkInfo.found) {
            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), ICON_FA_CIRCLE_CHECK " Found %s", m_emsdkInfo.description.c_str());
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " Not found");
        }
    });
}

bool EditorSettingsWindow::applySettings() {
    AppSettings::setEditorVSyncEnabled(m_editorVSyncEnabled);
    AppSettings::setDefaultExportDirectory(m_defaultExportDirectory);
    AppSettings::setCMakePath(m_cmakeOverride);
    LocalBuildSettings build;
    if (m_cmakeKitIndex > 0) {
        const auto& kit = m_availableKits[m_cmakeKitIndex - 1];
        build.cCompiler = kit.cCompiler;
        build.cxxCompiler = kit.cxxCompiler;
        build.generator = kit.generator;
    }
    if (m_project) {
        const auto file = m_project->getProjectPath() / "project.yaml";
        build.buildJobs = AppSettings::getBuildSettings(file).buildJobs;
        AppSettings::setBuildSettings(file, build);
    }
    // Also the editor-wide default, so a new project starts from it
    AppSettings::setLastCMakeKit(build.cCompiler, build.cxxCompiler, build.generator);
    AppSettings::setEmsdkPath(m_emsdkOverride);
    return AppSettings::saveSettings();
}

} // namespace doriax::editor
