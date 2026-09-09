// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "BundlesWindow.h"
#include "Theme.h"

#include <algorithm>

namespace doriax::editor {

static constexpr float dialogWidth = 600.0f;
static constexpr float dialogHeight = 420.0f;
static constexpr float dialogButtonWidth = 120.0f;
static constexpr ImGuiWindowFlags noScrollFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

void BundlesWindow::open(Project* project) {
    m_isOpen = true;
    m_project = project;
    m_entries.clear();

    for (const fs::path& bundlePath : project->findBundleFiles()) {
        BundleEntry entry;
        entry.path = bundlePath;

        for (const SceneProject& sceneProject : project->getScenes()) {
            const bool usesBundle = std::any_of(sceneProject.bundles.begin(), sceneProject.bundles.end(),
                [&](const BundleSceneInfo& info) { return info.bundlePath == bundlePath; });
            if (usesBundle) {
                entry.usedBy += entry.usedBy.empty() ? sceneProject.name : ", " + sceneProject.name;
            }
        }

        entry.included = !entry.usedBy.empty() || project->isStandaloneBundle(bundlePath);
        m_entries.push_back(std::move(entry));
    }
}

void BundlesWindow::show() {
    if (!m_isOpen) return;

    ImGui::OpenPopup("Bundles##BundlesModal");

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

    if (ImGui::BeginPopupModal("Bundles##BundlesModal", &m_isOpen, flags)) {
        if (!m_isOpen) {
            ImGui::CloseCurrentPopup();
        } else {
            drawBundles();
        }
        ImGui::EndPopup();
    }
}

void BundlesWindow::drawBundles() {
    const ImGuiStyle& style = ImGui::GetStyle();

    ImGui::TextWrapped("Bundles used by a scene are always built. Check the others to build them too, "
                       "so scripts can spawn them by name with BundleManager.");
    ImGui::Spacing();

    const float footerY = ImGui::GetWindowHeight() - style.WindowPadding.y - ImGui::GetFrameHeight();
    const float listHeight = std::max(1.0f, footerY - style.ItemSpacing.y * 2.0f - ImGui::GetCursorPosY());

    if (ImGui::BeginChild("##BundlesList", ImVec2(0, listHeight), ImGuiChildFlags_Borders)) {
        if (m_entries.empty()) {
            ImGui::TextDisabled("No bundle found in this project.");
        } else if (ImGui::BeginTable("##BundlesTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Bundle", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableSetupColumn("Used by", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableHeadersRow();

            for (BundleEntry& entry : m_entries) {
                const bool locked = !entry.usedBy.empty();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::BeginDisabled(locked);
                ImGui::Checkbox(entry.path.generic_string().c_str(), &entry.included);
                ImGui::EndDisabled();

                ImGui::TableNextColumn();
                if (locked) {
                    ImGui::TextUnformatted(entry.usedBy.c_str());
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
        applyBundles();
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }
}

void BundlesWindow::applyBundles() {
    if (!m_project) {
        return;
    }

    // Scene bundles are built through their scene, listing them would keep them after the last
    // instance is deleted
    std::vector<fs::path> bundlePaths;
    for (const BundleEntry& entry : m_entries) {
        if (entry.included && entry.usedBy.empty()) {
            bundlePaths.push_back(entry.path);
        }
    }

    m_project->setStandaloneBundles(std::move(bundlePaths));
    m_project->saveProject();
}

}
