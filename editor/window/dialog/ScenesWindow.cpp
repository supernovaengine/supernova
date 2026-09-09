// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ScenesWindow.h"
#include "Theme.h"
#include "Out.h"

#include <algorithm>

namespace doriax::editor {

static constexpr float dialogWidth = 600.0f;
static constexpr float dialogHeight = 420.0f;
static constexpr float dialogButtonWidth = 120.0f;
static constexpr ImGuiWindowFlags noScrollFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

void ScenesWindow::open(Project* project) {
    m_isOpen = true;
    m_project = project;
    m_entries.clear();

    for (const fs::path& scenePath : project->findSceneFiles()) {
        SceneEntry entry;
        entry.path = scenePath;
        entry.sceneId = project->findSceneByPath(scenePath);
        entry.included = entry.sceneId != NULL_PROJECT_SCENE;
        entry.wasIncluded = entry.included;

        if (entry.included) {
            const SceneProject* sceneProject = project->getScene(entry.sceneId);
            entry.opened = sceneProject && sceneProject->opened;

            for (const SceneProject& parent : project->getScenes()) {
                if (project->hasChildScene(parent.id, entry.sceneId)) {
                    entry.parents.push_back({parent.id, parent.name});
                }
            }
        }

        m_entries.push_back(std::move(entry));
    }
}

bool ScenesWindow::isParentKept(uint32_t parentSceneId) const {
    for (const SceneEntry& entry : m_entries) {
        if (entry.sceneId == parentSceneId) {
            return entry.included;
        }
    }

    // A scene with no file of its own is not listed here
    return true;
}

std::string ScenesWindow::keptParentNames(const SceneEntry& entry) const {
    std::string names;
    for (const ParentRef& parent : entry.parents) {
        if (isParentKept(parent.sceneId)) {
            names += names.empty() ? parent.name : ", " + parent.name;
        }
    }

    return names;
}

void ScenesWindow::show() {
    if (!m_isOpen) return;

    ImGui::OpenPopup("Scenes##ScenesModal");

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size(
        std::min(Theme::dpi(dialogWidth), viewport->WorkSize.x * 0.9f),
        std::min(Theme::dpi(dialogHeight), viewport->WorkSize.y * 0.9f)
    );
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_Modal |
                             ImGuiWindowFlags_NoResize |
                             noScrollFlags;

    if (ImGui::BeginPopupModal("Scenes##ScenesModal", &m_isOpen, flags)) {
        if (!m_isOpen) {
            ImGui::CloseCurrentPopup();
        } else {
            drawScenes();
        }
        ImGui::EndPopup();
    }
}

void ScenesWindow::drawScenes() {
    const ImGuiStyle& style = ImGui::GetStyle();

    ImGui::TextWrapped("Only the checked scenes are part of the project, built and exported. "
                       "Unchecking one keeps its file on disk.");
    ImGui::Spacing();

    // A kept parent brings its child back, and that child can be a parent too
    bool restored = true;
    while (restored) {
        restored = false;
        for (SceneEntry& entry : m_entries) {
            if (!entry.included && !keptParentNames(entry).empty()) {
                entry.included = true;
                restored = true;
            }
        }
    }

    // Scenes never saved to a file are not listed here
    int plannedCount = static_cast<int>(m_project->getScenes().size());
    for (const SceneEntry& entry : m_entries) {
        if (entry.included && !entry.wasIncluded) plannedCount++;
        else if (!entry.included && entry.wasIncluded) plannedCount--;
    }

    const float footerY = ImGui::GetWindowHeight() - style.WindowPadding.y - ImGui::GetFrameHeight();
    const float listHeight = std::max(1.0f, footerY - style.ItemSpacing.y * 2.0f - ImGui::GetCursorPosY());

    if (ImGui::BeginChild("##ScenesList", ImVec2(0, listHeight), ImGuiChildFlags_Borders)) {
        if (m_entries.empty()) {
            ImGui::TextDisabled("No scene file found in this project.");
        } else if (ImGui::BeginTable("##ScenesTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Scene", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableSetupColumn("Kept in the project", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableHeadersRow();

            for (SceneEntry& entry : m_entries) {
                const std::string usedBy = keptParentNames(entry);

                const char* keptReason = nullptr;
                if (!usedBy.empty()) {
                    keptReason = "Used as a child scene";
                } else if (entry.opened) {
                    keptReason = "Open in a tab, close the scene first";
                } else if (entry.included && plannedCount <= 1) {
                    keptReason = "The project needs at least one scene";
                }
                const bool locked = keptReason != nullptr;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::BeginDisabled(locked);
                ImGui::Checkbox(entry.path.generic_string().c_str(), &entry.included);
                ImGui::EndDisabled();

                ImGui::TableNextColumn();
                if (!usedBy.empty()) {
                    ImGui::Text("Child of %s", usedBy.c_str());
                } else if (locked) {
                    ImGui::TextDisabled("%s", keptReason);
                } else {
                    ImGui::TextDisabled("-");
                }
            }

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(style.WindowPadding.x, footerY - style.ItemSpacing.y));
    ImGui::Separator();

    const float footerWidth = std::max(1.0f, ImGui::GetWindowWidth() - style.WindowPadding.x * 2.0f);
    const float buttonWidth = std::clamp((footerWidth - style.ItemSpacing.x) * 0.5f, 1.0f, Theme::dpi(dialogButtonWidth));
    const float buttonsWidth = buttonWidth * 2.0f + style.ItemSpacing.x;
    ImGui::SetCursorPos(ImVec2(style.WindowPadding.x + std::max(0.0f, (footerWidth - buttonsWidth) * 0.5f), footerY));

    if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
        applyScenes();
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }
}

void ScenesWindow::applyScenes() {
    // Play can be started with F5 while this dialog is open
    if (m_project->isAnyScenePlaying()) {
        Out::warning("Cannot change the project scenes while a scene is playing");
        return;
    }

    bool changed = false;
    bool addFailed = false;

    // Added first so a swap is not refused for leaving no scene
    for (const SceneEntry& entry : m_entries) {
        if (!entry.included || entry.wasIncluded) {
            continue;
        }

        m_project->loadScene(entry.path, false, true, false);

        // loadScene only reports a broken file through the output
        if (m_project->findSceneByPath(entry.path) == NULL_PROJECT_SCENE) {
            addFailed = true;
            continue;
        }

        changed = true;
    }

    // A failed add plus a removal would drop the scene it was meant to replace
    if (addFailed) {
        Out::warning("A scene could not be added, keeping the listed scenes");
    } else {
        for (const SceneEntry& entry : m_entries) {
            if (!entry.included && entry.wasIncluded) {
                m_project->removeScene(entry.sceneId);
                changed = true;
            }
        }
    }

    if (changed) {
        m_project->saveProjectFile();
    }
}

}
