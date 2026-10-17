// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

// ProjectSaveDialog.cpp
#include "ProjectSaveDialog.h"
#include "util/FileDialogs.h"
#include "util/FileUtils.h"
#include "Backend.h"
#include "window/Widgets.h"
#include "external/IconsFontAwesome6.h"

#include <utility>

namespace doriax {
namespace editor {

void ProjectSaveDialog::open(const std::string& defaultName,
        std::function<void(const std::string&, const fs::path&)> onSave,
        std::function<void()> onCancel) {
    m_isOpen = true;
    m_projectName = defaultName;
    m_projectPath.clear();
    m_onSave = onSave;
    m_onCancel = onCancel;

    // Set default project name
    strncpy(m_projectNameBuffer, defaultName.c_str(), sizeof(m_projectNameBuffer) - 1);
    m_projectNameBuffer[sizeof(m_projectNameBuffer) - 1] = '\0'; // Ensure null termination
}

void ProjectSaveDialog::show() {
    if (!m_isOpen) {
        return;
    }

    // Setup modal popup
    ImGui::OpenPopup("Save Project##SaveProjectModal");

    // Center the popup
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Setup window flags for modal dialog
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_Modal;

    bool popupOpen = ImGui::BeginPopupModal("Save Project##SaveProjectModal", nullptr, flags);

    float popupWidth = 300;

    if (popupOpen) {
        // Project name input
        ImGui::Text("Project Name:");
        ImGui::SetNextItemWidth(popupWidth);
        ImGui::InputText("##projectName", m_projectNameBuffer, sizeof(m_projectNameBuffer));

        // Project path display and button
        ImGui::Text("Path:");

        Vector2 pathDisplaySize = Vector2(popupWidth - ImGui::CalcTextSize("Browse").x - ImGui::GetStyle().ItemSpacing.x - ImGui::GetStyle().FramePadding.x * 2, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2);
        Widgets::pathDisplay("##ProjectPath", m_projectPath, pathDisplaySize);

        ImGui::SameLine();
        if (ImGui::Button("Browse")) {
            // Get home directory as default path
            #ifdef _WIN32
            const wchar_t* homeDir = _wgetenv(L"USERPROFILE");
            #else
            const char* homeDir = std::getenv("HOME");
            #endif
            const std::string homeDirPath = homeDir ? FileUtils::pathToUtf8(fs::path(homeDir)) : "";
            
            // Open folder picker dialog
            std::string selectedPath = FileDialogs::openFileDialog(homeDirPath, 0, true);
            
            if (!selectedPath.empty()) {
                m_projectPath = FileUtils::pathFromUtf8(selectedPath);
            }
        }

        // Validation
        std::string projectName = m_projectNameBuffer;
        bool canSave = !projectName.empty() && !m_projectPath.empty();
        
        // Check if the selected directory is empty if it exists
        if (!m_projectPath.empty() && std::filesystem::exists(m_projectPath)) {
            if (!std::filesystem::is_empty(m_projectPath)) {
                canSave = false;
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
                                  "Selected directory is not empty.");
            }
        }

        ImGui::Separator();

        // Center the buttons
        float windowWidth = ImGui::GetWindowSize().x;
        float buttonsWidth = 250; // Total width for both buttons and spacing
        ImGui::SetCursorPosX((windowWidth - buttonsWidth) * 0.5f);

        // Buttons
        ImGui::BeginDisabled(!canSave);
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            auto onSave = std::exchange(m_onSave, nullptr);
            m_onCancel = nullptr;
            if (onSave) {
                onSave(projectName, m_projectPath);
            }
            m_isOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            cancel();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else {
        // If the popup isn't open anymore but our state says it should be,
        // update our state and make sure to call the cancel callback
        if (m_isOpen) {
            cancel();
        }
    }
}

void ProjectSaveDialog::close() {
    if (m_isOpen) {
        cancel();
    }
}

// Callbacks are cleared before running, so a closed dialog holds nothing of the save
void ProjectSaveDialog::cancel() {
    m_isOpen = false;

    auto onCancel = std::exchange(m_onCancel, nullptr);
    m_onSave = nullptr;
    if (onCancel) {
        onCancel();
    }
}

} // namespace editor
} // namespace doriax