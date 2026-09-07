// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#include "ExportWindow.h"
#include "util/FileDialogs.h"
#include "AppSettings.h"
#include "Backend.h"
#include "Generator.h"
#include "pool/ShaderPool.h"
#include "window/Widgets.h"
#include "external/IconsFontAwesome6.h"
#include "Out.h"
#include "Theme.h"

namespace doriax::editor {

namespace {
    // Desktop exports build for the current host, so only expose backends
    // supported by that host's standalone CMake configuration.
    #if defined(_WIN32)
    constexpr ShaderBackend desktopGraphicBackends[] = {
        ShaderBackend::GLCore, ShaderBackend::Vulkan, ShaderBackend::D3D11
    };
    #elif defined(__APPLE__)
    // Vulkan is left out: MoltenVK has no VK_EXT_descriptor_buffer, which the
    // renderer binds through, so an export would build and then fail at startup
    constexpr ShaderBackend desktopGraphicBackends[] = {
        ShaderBackend::MetalMacOS, ShaderBackend::GLCore
    };
    #else
    constexpr ShaderBackend desktopGraphicBackends[] = {
        ShaderBackend::GLCore, ShaderBackend::Vulkan
    };
    #endif

    constexpr int desktopGraphicBackendCount =
        static_cast<int>(sizeof(desktopGraphicBackends) / sizeof(desktopGraphicBackends[0]));

    float textButtonWidth(const char* label) {
        return ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    }

    void beginSettingsRow(const char* label) {
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFrameHeight());
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::TableNextColumn();
    }

    bool isHostBackend(ShaderBackend backend) {
        for (ShaderBackend hostBackend : desktopGraphicBackends) {
            if (hostBackend == backend) return true;
        }
        return false;
    }
}

void ExportWindow::open(Project* project) {
    m_isOpen = true;
    m_step = Step::ModeSelect;
    m_project = project;
    m_targetDir.clear();
    m_targetDirFromDefault = false;
    m_targetDirBuffer[0] = '\0';
    m_startSceneId = project->getStartSceneId();
    const SceneProject* startScene = project->getScene(m_startSceneId);
    if (!startScene || startScene->filepath.empty()) {
        m_startSceneId = NULL_PROJECT_SCENE;
        for (const auto& scene : project->getScenes()) {
            if (!scene.filepath.empty()) {
                m_startSceneId = scene.id;
                break;
            }
        }
    }
    m_selectedShaderIndex = -1;
    m_shaderKeysConfigured = false;
    m_addShaderOpen = false;
    m_emsdkOverride.clear();
    m_emsdkInfo = EmsdkInfo();
    m_missingBuildTools.clear();
    m_graphicBackendIndex = 0;
    m_sourceBackendsConfigured = false;
    m_desktopBackendConfigured = false;

    populateBackendList();
}

void ExportWindow::populateShaderList() {
    m_shaderEntries.clear();

    if (!m_project) return;

    // Collect all shaderKeys from all scenes (pre-populated list). The stored
    // keys are refreshed on scene save, so union them with a live collection —
    // otherwise a component added since the last save (e.g. a shadow-casting
    // Light2D) would be missing here and from the export.
    std::set<ShaderKey> addedKeys;
    for (const auto& scene : m_project->getScenes()) {
        std::set<ShaderKey> sceneKeys = scene.shaderKeys;
        m_project->collectSceneShaderKeys(&scene, sceneKeys);
        for (const auto& key : sceneKeys) {
            if (addedKeys.count(key)) continue;
            addedKeys.insert(key);

            ShaderType type = ShaderPool::getShaderTypeFromKey(key);
            uint32_t props = ShaderPool::getPropertiesFromKey(key);
            uint16_t customId = ShaderPool::getCustomIdFromKey(key);

            ShaderEntry entry;
            entry.key = key;
            entry.type = type;
            entry.properties = props;
            entry.displayName = Exporter::getShaderDisplayName(type, props, customId);
            m_shaderEntries.push_back(entry);
        }
    }

    // Sort by type then properties
    std::sort(m_shaderEntries.begin(), m_shaderEntries.end(), [](const ShaderEntry& a, const ShaderEntry& b) {
        if (a.type != b.type) return (int)a.type < (int)b.type;
        return a.properties < b.properties;
    });
}

void ExportWindow::populateShaderListFromKeys(const std::vector<ShaderKey>& shaderKeys) {
    m_shaderEntries.clear();

    for (ShaderKey key : shaderKeys) {
        ShaderKey normalizedKey = ShaderPool::normalizeKey(key);

        ShaderEntry entry;
        entry.key = normalizedKey;
        entry.type = ShaderPool::getShaderTypeFromKey(normalizedKey);
        entry.properties = ShaderPool::getPropertiesFromKey(normalizedKey);
        uint16_t customId = ShaderPool::getCustomIdFromKey(normalizedKey);
        entry.displayName = Exporter::getShaderDisplayName(entry.type, entry.properties, customId);
        m_shaderEntries.push_back(entry);
    }

    std::sort(m_shaderEntries.begin(), m_shaderEntries.end(), [](const ShaderEntry& a, const ShaderEntry& b) {
        if (a.type != b.type) return (int)a.type < (int)b.type;
        return a.properties < b.properties;
    });
}

void ExportWindow::loadShaderListFromSettings(const ExportTargetSettings& settings) {
    // Empty saved list should not hide the useful default shader list. It can
    // happen when only the export path was saved before shaders were collected.
    if (settings.shaderKeysConfigured && !settings.shaderKeys.empty()) {
        m_shaderKeysConfigured = true;
        populateShaderListFromKeys(settings.shaderKeys);
    } else {
        m_shaderKeysConfigured = false;
        populateShaderList();
    }
}

void ExportWindow::populateBackendList() {
    m_backendEntries.clear();

    for (ShaderBackend backend : ShaderPool::getShaderBackends()) {
        BackendEntry entry;
        entry.backend = backend;
        entry.name = ShaderPool::getShaderBackendName(backend);
        entry.selected = isHostBackend(backend);
        m_backendEntries.push_back(entry);
    }
}

void ExportWindow::refreshEmsdkStatus() {
    m_emsdkInfo = Exporter::detectEmsdk(m_emsdkOverride);
}

void ExportWindow::selectMode(ExportMode mode) {
    m_mode = mode;
    loadSettingsFromProject();
    // Both probes spawn processes, so run them once here rather than per-frame.
    if (mode == ExportMode::Desktop) {
        m_missingBuildTools = Generator::checkBuildTools();
    } else if (mode == ExportMode::Web) {
        refreshEmsdkStatus();
    }
    m_step = Step::Settings;
}

void ExportWindow::loadSettingsFromProject() {
    if (!m_project) return;

    m_targetDir.clear();
    m_targetDirFromDefault = false;
    m_targetDirBuffer[0] = '\0';
    m_selectedShaderIndex = -1;
    m_shaderKeysConfigured = false;
    m_sourceBackendsConfigured = false;
    m_sourcePlatformsConfigured = false;
    m_sourcePlatformWindows = false;
    m_sourcePlatformLinux = false;
    m_sourcePlatformMacOS = false;
    m_sourcePlatformIOS = false;
    m_sourcePlatformAndroid = false;
    m_sourcePlatformWeb = false;
    m_desktopBackendConfigured = false;

    if (m_mode == ExportMode::SourceCode) {
        const SourceCodeExportSettings& settings = m_project->getSourceCodeExportSettings();
        m_targetDir = settings.targetDir;
        loadShaderListFromSettings(settings);

        populateBackendList();
        m_sourceBackendsConfigured = settings.graphicBackendsConfigured;
        m_sourcePlatformsConfigured = settings.platformsConfigured;
        if (settings.platformsConfigured) {
            m_sourcePlatformWindows = settings.platformWindows;
            m_sourcePlatformLinux = settings.platformLinux;
            m_sourcePlatformMacOS = settings.platformMacOS;
            m_sourcePlatformIOS = settings.platformIOS;
            m_sourcePlatformAndroid = settings.platformAndroid;
            m_sourcePlatformWeb = settings.platformWeb;
        } else {
            #if defined(_WIN32)
            m_sourcePlatformWindows = true;
            #elif defined(__APPLE__)
            m_sourcePlatformMacOS = true;
            #else
            m_sourcePlatformLinux = true;
            #endif
        }
        if (settings.graphicBackendsConfigured) {
            for (auto& entry : m_backendEntries) {
                entry.selected = settings.graphicBackends.count(entry.backend) > 0;
            }
        }
    } else if (m_mode == ExportMode::Desktop) {
        const DesktopExportSettings& settings = m_project->getDesktopExportSettings();
        m_targetDir = settings.targetDir;
        loadShaderListFromSettings(settings);
        m_desktopBuildJobs = static_cast<int>(settings.buildJobs != 0
            ? settings.buildJobs
            : Generator::getAutomaticParallelBuildJobs());

        m_graphicBackendIndex = 0;
        m_desktopBackendConfigured = settings.graphicBackendConfigured;
        if (settings.graphicBackendConfigured) {
            for (int i = 0; i < desktopGraphicBackendCount; ++i) {
                if (desktopGraphicBackends[i] == settings.graphicBackend) {
                    m_graphicBackendIndex = i;
                    break;
                }
            }
        }
    } else if (m_mode == ExportMode::Web) {
        const WebExportSettings& settings = m_project->getWebExportSettings();
        m_targetDir = settings.targetDir;
        m_emsdkOverride = AppSettings::getEmsdkPath();
        loadShaderListFromSettings(settings);
    }

    if (m_targetDir.empty()) {
        m_targetDir = AppSettings::getDefaultExportDirectory();
        m_targetDirFromDefault = !m_targetDir.empty();
    }

    if (!m_targetDir.empty()) {
        std::string targetDirString = m_targetDir.string();
        strncpy(m_targetDirBuffer, targetDirString.c_str(), sizeof(m_targetDirBuffer) - 1);
        m_targetDirBuffer[sizeof(m_targetDirBuffer) - 1] = '\0';
    }
}

void ExportWindow::saveCurrentSettingsToProject(bool saveProjectFile) {
    if (!m_project) return;

    std::vector<ShaderKey> shaderKeys;
    shaderKeys.reserve(m_shaderEntries.size());
    for (const auto& entry : m_shaderEntries) {
        shaderKeys.push_back(entry.key);
    }

    if (m_mode == ExportMode::SourceCode) {
        SourceCodeExportSettings& settings = m_project->getSourceCodeExportSettings();
        if (!m_targetDirFromDefault) {
            settings.targetDir = m_targetDir;
        }
        settings.shaderKeys = shaderKeys;
        settings.shaderKeysConfigured = m_shaderKeysConfigured;
        settings.graphicBackends.clear();
        for (const auto& entry : m_backendEntries) {
            if (entry.selected) {
                settings.graphicBackends.insert(entry.backend);
            }
        }
        settings.graphicBackendsConfigured = m_sourceBackendsConfigured;
        settings.platformsConfigured = m_sourcePlatformsConfigured;
        settings.platformWindows = m_sourcePlatformWindows;
        settings.platformLinux = m_sourcePlatformLinux;
        settings.platformMacOS = m_sourcePlatformMacOS;
        settings.platformIOS = m_sourcePlatformIOS;
        settings.platformAndroid = m_sourcePlatformAndroid;
        settings.platformWeb = m_sourcePlatformWeb;
    } else if (m_mode == ExportMode::Desktop) {
        DesktopExportSettings& settings = m_project->getDesktopExportSettings();
        if (!m_targetDirFromDefault) {
            settings.targetDir = m_targetDir;
        }
        settings.shaderKeys = shaderKeys;
        settings.shaderKeysConfigured = m_shaderKeysConfigured;

        const int backendIndex =
            (m_graphicBackendIndex >= 0 && m_graphicBackendIndex < desktopGraphicBackendCount)
                ? m_graphicBackendIndex
                : 0;
        settings.graphicBackend = desktopGraphicBackends[backendIndex];
        settings.graphicBackendConfigured = m_desktopBackendConfigured;
        settings.buildJobs = static_cast<unsigned int>(m_desktopBuildJobs);
    } else if (m_mode == ExportMode::Web) {
        WebExportSettings& settings = m_project->getWebExportSettings();
        if (!m_targetDirFromDefault) {
            settings.targetDir = m_targetDir;
        }
        settings.emsdkPath.clear();
        settings.shaderKeys = shaderKeys;
        settings.shaderKeysConfigured = m_shaderKeysConfigured;
    }

    if (saveProjectFile) {
        m_project->saveProjectFile();
    }
}

void ExportWindow::show() {
    if (!m_isOpen) return;

    ImGui::OpenPopup("Export Project##ExportModal");

    float width = Theme::dpi((m_step == Step::ModeSelect) ? 640.0f : 550.0f);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(width, 0),
        ImVec2(width, ImGui::GetMainViewport()->WorkSize.y * 0.9f)
    );

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_Modal |
                             ImGuiWindowFlags_AlwaysAutoResize;

    bool popupOpen = ImGui::BeginPopupModal("Export Project##ExportModal", &m_isOpen, flags);

    if (popupOpen) {
        if (!m_isOpen) {
            // Closed via the title-bar X: kill a still-running export first.
            if (m_step == Step::Progress && m_exporter.isRunning()) {
                m_exporter.cancelExport();
            }
            ImGui::CloseCurrentPopup();
        } else if (m_step == Step::Progress) {
            drawProgress();
        } else if (m_step == Step::Settings) {
            drawSettings();
        } else {
            drawModeSelect();
        }
        ImGui::EndPopup();
    }
}

bool ExportWindow::drawModeCard(const char* id, const char* icon, const char* title, const char* description,
                                const ImVec2& size, const char* disabledText) {
    ImVec2 cardPos = ImGui::GetCursorPos();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.235f, 0.314f, 0.471f, 0.392f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.314f, 0.392f, 0.549f, 0.588f));
    bool clicked = ImGui::Button(id, size);
    ImGui::PopStyleColor(3);

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::GetWindowDrawList()->AddRect(
        ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
        ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetStyle().FrameRounding);

    // NOTE: sizes derive from FontSizeBase, not GetFontSize() — passing the
    // latter to PushFont would apply the global font scale twice.
    const float baseSize = ImGui::GetStyle().FontSizeBase;
    const float padX = Theme::dpi(12.0f);

    // Large centered icon
    ImGui::PushFont(ImGui::GetFont(), baseSize * 2.6f);
    ImVec2 iconSize = ImGui::CalcTextSize(icon);
    ImGui::SetCursorPos(ImVec2(cardPos.x + (size.x - iconSize.x) * 0.5f, cardPos.y + Theme::dpi(22.0f)));
    ImGui::TextUnformatted(icon);
    ImGui::PopFont();

    // Title
    ImGui::PushFont(ImGui::GetFont(), baseSize * 1.15f);
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    float titleY = cardPos.y + Theme::dpi(22.0f) + iconSize.y + Theme::dpi(14.0f);
    ImGui::SetCursorPos(ImVec2(cardPos.x + (size.x - titleSize.x) * 0.5f, titleY));
    ImGui::TextUnformatted(title);
    ImGui::PopFont();

    // Description
    const float descriptionY = titleY + titleSize.y + Theme::dpi(8.0f);
    const float wrapWidth = size.x - padX * 2.0f;
    ImGui::SetCursorPos(ImVec2(cardPos.x + padX, descriptionY));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    ImGui::PushTextWrapPos(cardPos.x + size.x - padX);
    ImGui::TextWrapped("%s", description);
    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();

    if (disabledText) {
        const float descriptionHeight = ImGui::CalcTextSize(description, nullptr, false, wrapWidth).y;
        ImGui::SetCursorPos(ImVec2(cardPos.x + padX, descriptionY + descriptionHeight + Theme::dpi(4.0f)));
        ImGui::PushTextWrapPos(cardPos.x + size.x - padX);
        ImGui::TextDisabled("%s", disabledText);
        ImGui::PopTextWrapPos();
    }

    // Re-anchor the layout cursor to the card rect so SameLine() places the
    // next card correctly after the overlay drawing above.
    ImGui::SetCursorPos(cardPos);
    ImGui::Dummy(size);

    return clicked;
}

void ExportWindow::drawModeSelect() {
    ImGui::Spacing();
    ImGui::TextDisabled("Choose how to export your project:");
    ImGui::Spacing();

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float cardWidth = (ImGui::GetContentRegionAvail().x - spacing * 2.0f) / 3.0f;
    const ImVec2 cardSize(cardWidth, Theme::dpi(185.0f));

    if (drawModeCard("##mode_source", ICON_FA_CODE, "Source Code",
                     "C++ engine source for every supported platform:", cardSize,
                     "Android \xc2\xb7 iOS \xc2\xb7 Web \xc2\xb7 Windows \xc2\xb7 Mac \xc2\xb7 Linux")) {
        selectMode(ExportMode::SourceCode);
    }
    ImGui::SameLine();
    if (drawModeCard("##mode_desktop", ICON_FA_DESKTOP, "Desktop",
                     "Build a ready-to-run native executable for this computer", cardSize)) {
        selectMode(ExportMode::Desktop);
    }
    ImGui::SameLine();
    if (drawModeCard("##mode_web", ICON_FA_GLOBE, "Web",
                     "Build an HTML and WebAssembly version using Emscripten", cardSize)) {
        selectMode(ExportMode::Web);
    }

    ImGui::Spacing();
    ImGui::Spacing();

    float windowWidth = ImGui::GetWindowSize().x;
    float cancelWidth = Theme::dpi(120.0f);
    ImGui::SetCursorPosX((windowWidth - cancelWidth) * 0.5f);
    if (ImGui::Button("Cancel", ImVec2(cancelWidth, 0))) {
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }
}

void ExportWindow::drawOutputDirRow(const char* label) {
    beginSettingsRow(label);
    {
        float browseWidth = ImGui::CalcTextSize("Browse").x + ImGui::GetStyle().FramePadding.x * 2;
        float inputWidth = ImGui::GetContentRegionAvail().x - browseWidth - ImGui::GetStyle().ItemSpacing.x;

        Vector2 pathSize = Vector2(inputWidth, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2);
        Widgets::pathDisplay("##TargetPath", m_targetDir, pathSize);

        ImGui::SameLine();
        if (ImGui::Button("Browse##target")) {
            std::string homeDirPath;
            #ifdef _WIN32
            const char* homeDir = getenv("USERPROFILE");
            #else
            const char* homeDir = getenv("HOME");
            #endif
            if (homeDir) homeDirPath = std::filesystem::path(homeDir).string();

            std::string startDir = m_targetDir.empty() ? homeDirPath : m_targetDir.string();
            std::string selectedPath = FileDialogs::openFileDialog(startDir, FILE_DIALOG_ALL, true);
            if (!selectedPath.empty()) {
                m_targetDir = selectedPath;
                m_targetDirFromDefault = false;
                strncpy(m_targetDirBuffer, selectedPath.c_str(), sizeof(m_targetDirBuffer) - 1);
                m_targetDirBuffer[sizeof(m_targetDirBuffer) - 1] = '\0';
                saveCurrentSettingsToProject();
            }
        }
    }
}

void ExportWindow::drawStartSceneRow() {
    beginSettingsRow("Start Scene");
    {
        const auto& scenes = m_project->getScenes();
        const SceneProject* selectedScene = m_project->getScene(m_startSceneId);
        if (!selectedScene || selectedScene->filepath.empty()) {
            selectedScene = nullptr;
            m_startSceneId = NULL_PROJECT_SCENE;
            for (const auto& scene : scenes) {
                if (!scene.filepath.empty()) {
                    selectedScene = &scene;
                    m_startSceneId = scene.id;
                    break;
                }
            }
        }

        if (selectedScene) {
            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##StartScene", selectedScene->name.c_str())) {
                for (const auto& scene : scenes) {
                    if (scene.filepath.empty()) continue;

                    bool isSelected = m_startSceneId == scene.id;
                    if (ImGui::Selectable(scene.name.c_str(), isSelected)) {
                        m_startSceneId = scene.id;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        } else {
            ImGui::TextDisabled("No saved scenes");
        }
    }
}

void ExportWindow::drawGraphicBackendRow() {
    beginSettingsRow("Graphic Backend");

    if (m_mode == ExportMode::Web) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("WebGL 2 (OpenGL ES 3)");
        ImGui::SetItemTooltip("Web exports currently use the WebGL 2 backend");
        return;
    }

    if (m_graphicBackendIndex < 0 || m_graphicBackendIndex >= desktopGraphicBackendCount) {
        m_graphicBackendIndex = 0;
    }

    const std::string preview = ShaderPool::getShaderBackendName(desktopGraphicBackends[m_graphicBackendIndex]);
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##GraphicBackend", preview.c_str())) {
        for (int i = 0; i < desktopGraphicBackendCount; ++i) {
            const bool selected = (m_graphicBackendIndex == i);
            if (ImGui::Selectable(ShaderPool::getShaderBackendName(desktopGraphicBackends[i]).c_str(), selected)) {
                m_graphicBackendIndex = i;
                m_desktopBackendConfigured = true;
                saveCurrentSettingsToProject();
            }
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void ExportWindow::drawDesktopKitRows() {
    std::string cCompiler = AppSettings::getLastCMakeCCompiler();
    std::string cxxCompiler = AppSettings::getLastCMakeCxxCompiler();
    std::string generator = AppSettings::getLastCMakeGenerator();

    std::string kitDisplay;
    if (cCompiler.empty() && cxxCompiler.empty() && generator.empty()) {
        kitDisplay = "Default (system toolchain)";
    } else {
        kitDisplay = !cxxCompiler.empty() ? cxxCompiler : cCompiler;
        if (!generator.empty()) {
            kitDisplay += kitDisplay.empty() ? generator : " (" + generator + ")";
        }
    }

    beginSettingsRow("Compiler");
    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("%s", kitDisplay.c_str());
    ImGui::SetItemTooltip("Change in Editor Settings");

    beginSettingsRow("Build Jobs");
    ImGui::SetNextItemWidth(-1);
    const int previousJobs = m_desktopBuildJobs;
    if (ImGui::InputInt("##DesktopBuildJobs", &m_desktopBuildJobs, 1, 8)) {
        m_desktopBuildJobs = std::clamp(m_desktopBuildJobs, 1, static_cast<int>(Generator::MAX_SUPPORTED_PARALLEL_BUILD_JOBS));
        if (m_desktopBuildJobs != previousJobs) {
            saveCurrentSettingsToProject();
        }
    }
    ImGui::SetItemTooltip("Parallel build jobs. Default: %u (automatic maximum).", Generator::getAutomaticParallelBuildJobs());
}

void ExportWindow::drawShaderSection() {
    ImGui::Text(ICON_FA_WAND_MAGIC_SPARKLES "  Shaders");
    ImGui::SameLine();

    const char* addLabel = ICON_FA_PLUS " Add";
    const char* deleteLabel = ICON_FA_TRASH " Delete";
    float buttonsGroupWidth = textButtonWidth(addLabel) + textButtonWidth(deleteLabel) + ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - buttonsGroupWidth);

    if (ImGui::Button(addLabel)) {
        m_addShaderOpen = true;
        m_addShaderTypeIndex = 0;
        memset(m_addShaderProps, 0, sizeof(m_addShaderProps));
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(m_selectedShaderIndex < 0 || m_selectedShaderIndex >= (int)m_shaderEntries.size());
    if (ImGui::Button(deleteLabel)) {
        m_shaderEntries.erase(m_shaderEntries.begin() + m_selectedShaderIndex);
        m_selectedShaderIndex = -1;
        m_shaderKeysConfigured = true;
        saveCurrentSettingsToProject();
    }
    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginChild("ShaderList", ImVec2(0, Theme::dpi(160.0f)), true);
    {
        for (int i = 0; i < (int)m_shaderEntries.size(); i++) {
            const auto& entry = m_shaderEntries[i];
            bool isSelected = (m_selectedShaderIndex == i);

            if (ImGui::Selectable(entry.displayName.c_str(), isSelected)) {
                m_selectedShaderIndex = i;
            }
        }
    }
    ImGui::EndChild();

    drawAddShaderDialog();
}

void ExportWindow::applySourcePlatformPresets() {
    std::set<ShaderBackend> required;
    auto add = [&](std::initializer_list<ShaderBackend> backends) {
        required.insert(backends.begin(), backends.end());
    };
    if (m_sourcePlatformWindows) add({ShaderBackend::GLCore, ShaderBackend::Vulkan, ShaderBackend::D3D11});
    if (m_sourcePlatformLinux) add({ShaderBackend::GLCore, ShaderBackend::Vulkan});
    if (m_sourcePlatformMacOS) add({ShaderBackend::MetalMacOS, ShaderBackend::GLCore});
    if (m_sourcePlatformIOS) add({ShaderBackend::MetalIOS});
    if (m_sourcePlatformAndroid) add({ShaderBackend::GLES3, ShaderBackend::Vulkan});
    if (m_sourcePlatformWeb) add({ShaderBackend::GLES3});

    for (auto& entry : m_backendEntries) {
        entry.selected = required.count(entry.backend) > 0;
    }
    m_sourceBackendsConfigured = true;
}

void ExportWindow::drawBackendSection() {
    ImGui::Text(ICON_FA_LAYER_GROUP "  Platform Presets");
    ImGui::Spacing();
    if (ImGui::BeginTable("source_platforms_table", 3, ImGuiTableFlags_SizingStretchSame)) {
        auto platformCheck = [&](const char* label, bool& value) {
            ImGui::TableNextColumn();
            if (ImGui::Checkbox(label, &value)) {
                m_sourcePlatformsConfigured = true;
                applySourcePlatformPresets();
                saveCurrentSettingsToProject();
            }
        };
        platformCheck("Windows", m_sourcePlatformWindows);
        platformCheck("Linux", m_sourcePlatformLinux);
        platformCheck("macOS", m_sourcePlatformMacOS);
        platformCheck("iOS", m_sourcePlatformIOS);
        platformCheck("Android", m_sourcePlatformAndroid);
        platformCheck("Web", m_sourcePlatformWeb);
        ImGui::EndTable();
    }
    ImGui::TextDisabled("A preset selects all compatible shader backends. You can still adjust individual backends below.");
    ImGui::Spacing();

    ImGui::Text(ICON_FA_MICROCHIP "  Graphic Backends");
    ImGui::Spacing();

    if (ImGui::BeginTable("backends_table", 3, ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("BackendCol1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("BackendCol2", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("BackendCol3", ImGuiTableColumnFlags_WidthStretch);
        for (auto& entry : m_backendEntries) {
            ImGui::TableNextColumn();
            std::string checkboxId = "##backend_" + entry.name;
            if (ImGui::Checkbox((entry.name + checkboxId).c_str(), &entry.selected)) {
                m_sourceBackendsConfigured = true;
                saveCurrentSettingsToProject();
            }
            ImGui::SetItemTooltip("%s", ShaderPool::getShaderLangStr(entry.backend).c_str());
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void ExportWindow::drawSettings() {
    // Mode header
    const char* modeIcon = ICON_FA_CODE;
    const char* modeTitle = "Source Code Export";
    if (m_mode == ExportMode::Desktop) {
        modeIcon = ICON_FA_DESKTOP;
        modeTitle = "Desktop Export";
    } else if (m_mode == ExportMode::Web) {
        modeIcon = ICON_FA_GLOBE;
        modeTitle = "Web Export";
    }
    ImGui::Text("%s  %s", modeIcon, modeTitle);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushItemWidth(-1);
    ImGui::BeginTable("export_settings", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp);
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, Theme::dpi(140.0f));
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

    drawOutputDirRow(m_mode == ExportMode::SourceCode ? "Target Directory" : "Destination Directory");
    drawStartSceneRow();
    if (m_mode == ExportMode::Desktop) {
        drawGraphicBackendRow();
        drawDesktopKitRows();
    } else if (m_mode == ExportMode::Web) {
        drawGraphicBackendRow();
    }

    ImGui::EndTable();
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    drawShaderSection();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (m_mode == ExportMode::SourceCode) {
        drawBackendSection();
    }

    // --- Validation warnings ---
    bool canExport = !m_targetDir.empty();
    std::error_code ec;
    bool targetExists = !m_targetDir.empty() && fs::exists(m_targetDir, ec);
    bool targetNotEmpty = targetExists && !fs::is_empty(m_targetDir, ec);

    if (targetNotEmpty) {
        const char* dirLabel = (m_mode == ExportMode::SourceCode) ? "Target directory" : "Destination";
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " %s is not empty: existing files will be overwritten", dirLabel);
    }

    bool hasSavedScenes = false;
    for (const auto& scene : m_project->getScenes()) {
        if (!scene.filepath.empty()) {
            hasSavedScenes = true;
            break;
        }
    }

    if (!hasSavedScenes) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " No saved scenes in project");
        canExport = false;
    } else if (m_shaderEntries.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " No shaders in list");
    }

    if (m_mode == ExportMode::SourceCode) {
        bool hasSelectedBackends = false;
        for (const auto& entry : m_backendEntries) {
            if (entry.selected) { hasSelectedBackends = true; break; }
        }
        if (!hasSelectedBackends) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " No graphic backends selected");
            canExport = false;
        }
    } else if (m_mode == ExportMode::Desktop) {
        if (!m_missingBuildTools.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            ImGui::TextWrapped(ICON_FA_TRIANGLE_EXCLAMATION " Missing build tools:\n%s", m_missingBuildTools.c_str());
            ImGui::PopStyleColor();
            canExport = false;
        }
    } else if (m_mode == ExportMode::Web) {
        if (!m_emsdkInfo.found) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            ImGui::TextWrapped(ICON_FA_TRIANGLE_EXCLAMATION " Emscripten SDK not found. Configure it in Editor Settings > Web.");
            ImGui::PopStyleColor();
            canExport = false;
        }
    }

    // --- Buttons ---
    ImGui::Spacing();
    if (ImGui::Button(ICON_FA_ARROW_LEFT " Back")) {
        m_step = Step::ModeSelect;
    }

    ImGui::SameLine();
    const float actionWidth = Theme::dpi(120.0f);
    float buttonsWidth = actionWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - buttonsWidth);

    ImGui::BeginDisabled(!canExport);
    if (ImGui::Button("Export", ImVec2(actionWidth, 0))) {
        if (targetNotEmpty) {
            ImGui::OpenPopup("Directory Not Empty##ExportOverwrite");
        } else {
            startConfiguredExport(false);
        }
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(actionWidth, 0))) {
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }

    drawOverwriteConfirmDialog();
}

void ExportWindow::drawOverwriteConfirmDialog() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(Theme::dpi(ImVec2(440.0f, 0.0f)), ImVec2(Theme::dpi(440.0f), FLT_MAX));

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_Modal;

    if (!ImGui::BeginPopupModal("Directory Not Empty##ExportOverwrite", nullptr, flags)) return;

    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " This directory already contains files:");
    ImGui::Spacing();

    Vector2 pathSize = Vector2(ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2);
    Widgets::pathDisplay("##OverwritePath", m_targetDir, pathSize);

    ImGui::Spacing();
    ImGui::PushTextWrapPos();
    ImGui::Text("Files with the same name will be overwritten.");
    if (m_mode == ExportMode::SourceCode) {
        ImGui::TextDisabled("Files left from an earlier export are not removed and may break the build.");
    }
    ImGui::PopTextWrapPos();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float windowWidth = ImGui::GetWindowSize().x;
    const float actionWidth = Theme::dpi(120.0f);
    float buttonsWidth = actionWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX((windowWidth - buttonsWidth) * 0.5f);

    if (ImGui::Button("Overwrite", ImVec2(actionWidth, 0))) {
        ImGui::CloseCurrentPopup();
        startConfiguredExport(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(actionWidth, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void ExportWindow::startConfiguredExport(bool overwriteTarget) {
    ExportConfig exportConfig;
    exportConfig.mode = m_mode;
    exportConfig.overwriteTarget = overwriteTarget;

    if (m_mode == ExportMode::SourceCode) {
        exportConfig.targetDir = m_targetDir;
        exportConfig.packNativeResources = m_project->shouldPackNativeResources();
        for (const auto& entry : m_backendEntries) {
            if (entry.selected) {
                exportConfig.selectedBackends.insert(entry.backend);
            }
        }
    } else {
        exportConfig.destinationDir = m_targetDir;

        // A built export runs on a single backend, driving both its shaders and its CMake
        ShaderBackend backend = ShaderBackend::GLES3;
        if (m_mode == ExportMode::Desktop) {
            const int backendIndex =
                (m_graphicBackendIndex >= 0 && m_graphicBackendIndex < desktopGraphicBackendCount)
                    ? m_graphicBackendIndex
                    : 0;
            backend = desktopGraphicBackends[backendIndex];
        }
        exportConfig.selectedBackends.insert(backend);
        exportConfig.graphicBackend = Exporter::getCMakeGraphicBackend(backend);

        if (m_mode == ExportMode::Desktop) {
            exportConfig.cmakeCCompiler = AppSettings::getLastCMakeCCompiler();
            exportConfig.cmakeCxxCompiler = AppSettings::getLastCMakeCxxCompiler();
            exportConfig.cmakeGenerator = AppSettings::getLastCMakeGenerator();
            exportConfig.buildJobs = static_cast<unsigned int>(m_desktopBuildJobs);
            exportConfig.packNativeResources = m_project->shouldPackNativeResources();
        } else {
            exportConfig.emsdkPath = m_emsdkOverride;
        }
    }

    // Stored references are relative to these roots, set in the project settings
    exportConfig.assetsDir = m_project->getAssetsPath();
    exportConfig.luaDir = m_project->getLuaPath();

    // Set start scene
    const SceneProject* startScene = m_project->getScene(m_startSceneId);
    if (startScene && !startScene->filepath.empty()) {
        exportConfig.startSceneId = startScene->id;
        m_project->setStartSceneId(exportConfig.startSceneId);
    }

    for (const auto& entry : m_shaderEntries) {
        exportConfig.selectedShaderKeys.insert(entry.key);
    }

    m_shaderKeysConfigured = true;
    if (m_mode == ExportMode::SourceCode) {
        m_sourceBackendsConfigured = true;
    } else if (m_mode == ExportMode::Desktop) {
        m_desktopBackendConfigured = true;
    }

    m_step = Step::Progress;
    saveCurrentSettingsToProject();
    m_exporter.startExport(m_project, exportConfig);
}

void ExportWindow::drawAddShaderDialog() {
    if (!m_addShaderOpen) return;

    ImGui::OpenPopup("Add Shader##AddShaderModal");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_Modal;

    if (ImGui::BeginPopupModal("Add Shader##AddShaderModal", &m_addShaderOpen, flags)) {
        // Shader type combo. Keep in sync with the ShaderType enum
        // (engine/core/render/Render.h); names come from ShaderPool so the
        // labels never drift.
        static const ShaderType typeValues[] = {
            ShaderType::POINTS, ShaderType::LINES, ShaderType::MESH, ShaderType::SKYBOX,
            ShaderType::DEPTH, ShaderType::GBUFFER, ShaderType::UI,
            ShaderType::SSAO, ShaderType::SSAO_BLUR,
            ShaderType::SSR, ShaderType::SSR_BLUR, ShaderType::COMPOSITE,
            ShaderType::SHADOW2D, ShaderType::BLIT, ShaderType::POSTPROCESS
        };
        constexpr int typeCount = (int)(sizeof(typeValues) / sizeof(typeValues[0]));
        std::vector<std::string> typeNameStrs;
        std::vector<const char*> typeNames;
        for (ShaderType type : typeValues) {
            typeNameStrs.push_back(ShaderPool::getShaderTypeName(type, false));
        }
        for (const std::string& name : typeNameStrs) {
            typeNames.push_back(name.c_str());
        }

        ImGui::Text("Type");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::Combo("##shader_type", &m_addShaderTypeIndex, typeNames.data(), typeCount)) {
            // Reset properties when type changes
            memset(m_addShaderProps, 0, sizeof(m_addShaderProps));
        }

        // Property checkboxes
        ShaderType selectedType = typeValues[m_addShaderTypeIndex];
        int propCount = ShaderPool::getShaderPropertyCount(selectedType);

        if (propCount > 0) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Properties:");
            ImGui::Spacing();

            // Determine column count based on properties
            int columns = 1;
            if (propCount >= 6) columns = 2;
            if (propCount >= 12) columns = 3;

            if (ImGui::BeginTable("shader_props_table", columns, ImGuiTableFlags_NoBordersInBody)) {
                for (int i = 0; i < propCount; i++) {
                    ImGui::TableNextColumn();
                    std::string abbrev = ShaderPool::getShaderPropertyName(selectedType, i, true);
                    std::string fullName = ShaderPool::getShaderPropertyName(selectedType, i, false);
                    std::string label = fullName + " (" + abbrev + ")##prop_" + std::to_string(i);
                    ImGui::Checkbox(label.c_str(), &m_addShaderProps[i]);
                }
                ImGui::EndTable();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Preview
        uint32_t props = 0;
        for (int i = 0; i < propCount; i++) {
            if (m_addShaderProps[i]) props |= (1 << i);
        }
        std::string preview = Exporter::getShaderDisplayName(selectedType, props);
        ImGui::Text("Preview: %s", preview.c_str());

        ImGui::Spacing();

        // Check for duplicate
        ShaderKey newKey = ShaderPool::getShaderKey(selectedType, props);
        bool isDuplicate = false;
        for (const auto& entry : m_shaderEntries) {
            if (entry.key == newKey) { isDuplicate = true; break; }
        }
        if (isDuplicate) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "This shader already exists in the list");
        }

        // Buttons
        float windowWidth = ImGui::GetWindowSize().x;
        const float actionWidth = Theme::dpi(120.0f);
        float buttonsWidth = actionWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetCursorPosX((windowWidth - buttonsWidth) * 0.5f);

        ImGui::BeginDisabled(isDuplicate);
        if (ImGui::Button("Add", ImVec2(actionWidth, 0))) {
            ShaderEntry entry;
            entry.key = newKey;
            entry.type = selectedType;
            entry.properties = props;
            entry.displayName = preview;
            m_shaderEntries.push_back(entry);

            // Re-sort
            std::sort(m_shaderEntries.begin(), m_shaderEntries.end(), [](const ShaderEntry& a, const ShaderEntry& b) {
                if (a.type != b.type) return (int)a.type < (int)b.type;
                return a.properties < b.properties;
            });

            m_shaderKeysConfigured = true;
            saveCurrentSettingsToProject();
            m_addShaderOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(actionWidth, 0))) {
            m_addShaderOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    } else {
        m_addShaderOpen = false;
    }
}

void ExportWindow::drawProgress() {
    ExportProgress progress = m_exporter.getProgress();

    // The export thread is not input: without this the dialog would only advance on
    // mouse moves, and the finished layout would wait for one.
    if (!progress.finished && !progress.failed) {
        Backend::getApp().requestRedraw();
    }

    ImGui::Dummy(ImVec2(0.0f, Theme::dpi(6.0f)));
    ImGui::Text("Exporting project...");
    ImGui::Spacing();

    ImGui::ProgressBar(progress.overallProgress, ImVec2(-1.0f, 0.0f));

    ImGui::Spacing();
    ImGui::Text("%s", progress.currentStep.c_str());

    // Last build-output line, clipped to a single line so the modal keeps its size.
    ImGui::BeginChild("##BuildDetail", ImVec2(0, ImGui::GetTextLineHeight() + Theme::dpi(2.0f)), false, ImGuiWindowFlags_NoScrollbar);
    if (!progress.detailLine.empty()) {
        ImGui::TextDisabled("%s", progress.detailLine.c_str());
    }
    ImGui::EndChild();
    ImGui::Dummy(ImVec2(0.0f, Theme::dpi(6.0f)));

    if (progress.failed) {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::TextWrapped(ICON_FA_CIRCLE_XMARK " Export failed: %s", progress.errorMessage.c_str());
        ImGui::PopStyleColor();
        ImGui::Spacing();

        float windowWidth = ImGui::GetWindowSize().x;
        float closeWidth = Theme::dpi(120.0f);
        ImGui::SetCursorPosX((windowWidth - closeWidth) * 0.5f);
        if (ImGui::Button("Close", ImVec2(closeWidth, 0))) {
            m_isOpen = false;
            ImGui::CloseCurrentPopup();
        }
    } else if (progress.finished) {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), ICON_FA_CIRCLE_CHECK " Export completed successfully!");
        ImGui::Spacing();

        float windowWidth = ImGui::GetWindowSize().x;
        float closeWidth = Theme::dpi(120.0f);
        ImGui::SetCursorPosX((windowWidth - closeWidth) * 0.5f);
        if (ImGui::Button("Close", ImVec2(closeWidth, 0))) {
            m_isOpen = false;
            ImGui::CloseCurrentPopup();
        }
    } else {
        // Still running: cancelling also terminates the compiler subprocess.
        float windowWidth = ImGui::GetWindowSize().x;
        float cancelWidth = Theme::dpi(120.0f);
        ImGui::SetCursorPosX((windowWidth - cancelWidth) * 0.5f);
        if (ImGui::Button("Cancel", ImVec2(cancelWidth, 0))) {
            m_exporter.cancelExport();
        }
    }
}

} // namespace doriax::editor
