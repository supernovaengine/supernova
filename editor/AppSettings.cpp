// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "AppSettings.h"
#include "Out.h"
#include "ai/SecretStore.h"
#include <fstream>
#include <algorithm>
#include <cstdlib>

namespace doriax::editor {

namespace {

std::filesystem::path userConfigDirectory() {
#if defined(_WIN32)
    if (const char* appData = std::getenv("APPDATA"); appData && *appData) {
        return std::filesystem::path(appData) / "Doriax";
    }
#elif defined(__APPLE__)
    if (const char* home = std::getenv("HOME"); home && *home) {
        return std::filesystem::path(home) / "Library" / "Application Support" / "Doriax";
    }
#else
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg && *xdg) {
        return std::filesystem::path(xdg) / "doriax";
    }
    if (const char* home = std::getenv("HOME"); home && *home) {
        return std::filesystem::path(home) / ".config" / "doriax";
    }
#endif
    return std::filesystem::current_path();
}

bool isWritableDirectory(const std::filesystem::path& dir) {
    const std::filesystem::path probe = dir / ".doriax_write_test";
    std::error_code ec;
    {
        std::ofstream f(probe);
        if (!f.is_open()) return false;
    }
    std::filesystem::remove(probe, ec);
    return true;
}

// Upgrades the single "custom_endpoint" URL to the named endpoint list. An empty
// URL used to mean OpenRouter.
void migrateLegacyEndpoint(ai::Settings& settings, const std::string& url) {
    const std::string legacyAccount = ai::toString(ai::ProviderId::OpenAICompatible);
    if (url.empty() && settings.provider != ai::ProviderId::OpenAICompatible &&
        !ai::SecretStore::hasApiKey(legacyAccount)) {
        return; // never configured; do not invent an entry
    }

    ai::CustomEndpoint endpoint;
    endpoint.id = "custom";
    endpoint.label = "Custom endpoint";
    endpoint.url = url;
    for (const ai::EndpointPreset& preset : ai::endpointPresets()) {
        if (url.empty() ? preset.id == "openrouter" : preset.url == url) {
            endpoint = {preset.id, preset.label, preset.url};
            break;
        }
    }

    settings.customEndpoints.push_back(endpoint);
    if (settings.provider == ai::ProviderId::OpenAICompatible) {
        settings.endpointId = endpoint.id;
    }
    ai::SecretStore::renameAccount(legacyAccount,
                                   ai::accountKey(ai::ProviderId::OpenAICompatible, endpoint.id));
}

} // namespace

// Initialize static members
std::filesystem::path AppSettings::configFilePath;
YAML::Node AppSettings::settingsData;
std::vector<std::filesystem::path> AppSettings::recentProjects;
std::filesystem::path AppSettings::lastProjectPath;
std::string AppSettings::lastCMakeCCompiler;
std::string AppSettings::lastCMakeCxxCompiler;
std::string AppSettings::lastCMakeGenerator;
std::string AppSettings::emsdkPath;
std::string AppSettings::cmakePath;
std::filesystem::path AppSettings::defaultExportDirectory;
int AppSettings::windowWidth = 1280;
int AppSettings::windowHeight = 720;
bool AppSettings::isMaximized = false;
float AppSettings::windowUiScale = 0.0f;
float AppSettings::layoutUiScale = 0.0f;
int AppSettings::resourcesIconSize = 32;
int AppSettings::resourcesLayout = 0;
int AppSettings::resourcesItemViewStyle = 1;
float AppSettings::resourcesLeftPanelWidth = 200.0f;
float AppSettings::codeEditorFontSize = AppSettings::defaultCodeEditorFontSize;
bool AppSettings::multiViewportEnabled = false;
bool AppSettings::editorVSyncEnabled = true;
PanelVisibilitySettings AppSettings::panelVisibility;
ai::Settings AppSettings::aiSettings;

bool AppSettings::initialize() {
    // Settings live in the working directory, where a run from a build tree
    // keeps them. A desktop launcher starts the app in "/" instead, so there
    // they go to the per-user config directory: nothing could be saved
    // otherwise.
    const std::filesystem::path workingDir = std::filesystem::current_path();
    const std::filesystem::path workingDirSettings = workingDir / "settings.yaml";

    std::error_code ec;
    if (std::filesystem::exists(workingDirSettings, ec)) {
        configFilePath = workingDirSettings;
    } else {
        const std::filesystem::path userDir = userConfigDirectory();
        if (std::filesystem::exists(userDir / "settings.yaml", ec) || !isWritableDirectory(workingDir)) {
            configFilePath = userDir / "settings.yaml";
        } else {
            configFilePath = workingDirSettings;
        }
    }

    ensureConfigDirectory();
    return loadSettings();
}

std::filesystem::path AppSettings::getConfigDirectory() {
    if (configFilePath.empty()) {
        return std::filesystem::current_path();
    }
    return configFilePath.parent_path();
}

void AppSettings::ensureConfigDirectory() {
    std::filesystem::path configDir = configFilePath.parent_path();
    if (!std::filesystem::exists(configDir)) {
        try {
            std::filesystem::create_directories(configDir);
        } catch (const std::exception& e) {
            Out::error("Failed to create config directory: %s", e.what());
        }
    }
}

bool AppSettings::loadSettings() {
    try {
        if (!std::filesystem::exists(configFilePath)) {
            settingsData = YAML::Node();
            return false;
        }
        
        settingsData = YAML::LoadFile(configFilePath.string());
        
        // Load last project path
        if (settingsData["last_project_path"]) {
            std::string path = settingsData["last_project_path"].as<std::string>();
            if (!path.empty() && std::filesystem::exists(path)) {
                lastProjectPath = std::filesystem::path(path);
            }
        }
        
        // Load last compiler kit
        if (settingsData["cmake_kit"]) {
            auto kitNode = settingsData["cmake_kit"];
            if (kitNode["c_compiler"]) lastCMakeCCompiler = kitNode["c_compiler"].as<std::string>();
            if (kitNode["cxx_compiler"]) lastCMakeCxxCompiler = kitNode["cxx_compiler"].as<std::string>();
            if (kitNode["generator"]) lastCMakeGenerator = kitNode["generator"].as<std::string>();
        }

        // Load Emscripten SDK path override
        if (settingsData["emsdk"] && settingsData["emsdk"]["path"]) {
            emsdkPath = settingsData["emsdk"]["path"].as<std::string>();
        }

        // Load cmake executable override
        if (settingsData["cmake"] && settingsData["cmake"]["path"]) {
            cmakePath = settingsData["cmake"]["path"].as<std::string>();
        }
        if (settingsData["export"] && settingsData["export"]["default_dir"]) {
            defaultExportDirectory = settingsData["export"]["default_dir"].as<std::string>();
        }

        // Load recent projects
        recentProjects.clear();
        if (settingsData["recent_projects"]) {
            for (const auto& path : settingsData["recent_projects"]) {
                std::string pathStr = path.as<std::string>();
                if (!pathStr.empty() && std::filesystem::exists(pathStr)) {
                    recentProjects.push_back(std::filesystem::path(pathStr));
                }
            }
        }
        
        // Load window settings
        if (settingsData["window"]) {
            auto windowNode = settingsData["window"];
            if (windowNode["width"]) {
                windowWidth = windowNode["width"].as<int>();
            }
            if (windowNode["height"]) {
                windowHeight = windowNode["height"].as<int>();
            }
            if (windowNode["maximized"]) {
                isMaximized = windowNode["maximized"].as<bool>();
            }
            if (windowNode["ui_scale"]) {
                windowUiScale = windowNode["ui_scale"].as<float>();
            }
            if (windowNode["layout_ui_scale"]) {
                layoutUiScale = windowNode["layout_ui_scale"].as<float>();
            }
        }
        
        // Load resources window settings
        if (settingsData["resources_window"]) {
            auto resNode = settingsData["resources_window"];
            if (resNode["icon_size"]) {
                resourcesIconSize = resNode["icon_size"].as<int>();
            }
            if (resNode["layout"]) {
                resourcesLayout = resNode["layout"].as<int>();
            }
            if (resNode["item_view_style"]) {
                resourcesItemViewStyle = resNode["item_view_style"].as<int>();
            }
            if (resNode["left_panel_width"]) {
                resourcesLeftPanelWidth = resNode["left_panel_width"].as<float>();
            }
        }

        // Load code editor settings
        if (settingsData["code_editor"]) {
            auto codeNode = settingsData["code_editor"];
            if (codeNode["font_size"]) {
                setCodeEditorFontSize(codeNode["font_size"].as<float>());
            }
        }

        // Load editor viewport settings
        if (settingsData["editor"]) {
            auto editorNode = settingsData["editor"];
            if (editorNode["multi_viewport"]) {
                multiViewportEnabled = editorNode["multi_viewport"].as<bool>();
            }
            if (editorNode["vsync"]) {
                editorVSyncEnabled = editorNode["vsync"].as<bool>();
            }
            if (editorNode["panels"]) {
                auto panelsNode = editorNode["panels"];
                if (panelsNode["structure"]) panelVisibility.structure = panelsNode["structure"].as<bool>();
                if (panelsNode["properties"]) panelVisibility.properties = panelsNode["properties"].as<bool>();
                if (panelsNode["resources"]) panelVisibility.resources = panelsNode["resources"].as<bool>();
                if (panelsNode["output"]) panelVisibility.output = panelsNode["output"].as<bool>();
                if (panelsNode["animation"]) panelVisibility.animation = panelsNode["animation"].as<bool>();
                if (panelsNode["terrain"]) panelVisibility.terrain = panelsNode["terrain"].as<bool>();
                if (panelsNode["ai_chat"]) panelVisibility.aiChat = panelsNode["ai_chat"].as<bool>();
            }
        }

        // Load AI assistant settings (no API keys)
        if (settingsData["ai_assistant"]) {
            auto aiNode = settingsData["ai_assistant"];
            if (aiNode["provider"]) aiSettings.provider = ai::providerFromString(aiNode["provider"].as<std::string>());
            if (aiNode["model"]) aiSettings.model = aiNode["model"].as<std::string>();
            if (aiNode["endpoint_id"]) aiSettings.endpointId = aiNode["endpoint_id"].as<std::string>();
            if (aiNode["custom_endpoints"] && aiNode["custom_endpoints"].IsSequence()) {
                // Replace, not append: initialize() runs on the CLI and App paths.
                aiSettings.customEndpoints.clear();
                for (const auto& endpointNode : aiNode["custom_endpoints"]) {
                    ai::CustomEndpoint endpoint;
                    if (endpointNode["id"]) endpoint.id = endpointNode["id"].as<std::string>();
                    if (endpointNode["label"]) endpoint.label = endpointNode["label"].as<std::string>();
                    if (endpointNode["url"]) endpoint.url = endpointNode["url"].as<std::string>();
                    // Duplicate ids would share one key and one model list.
                    if (endpoint.id.empty() || ai::findEndpoint(aiSettings, endpoint.id)) continue;
                    aiSettings.customEndpoints.push_back(endpoint);
                }
            }
            if (aiNode["custom_endpoint"] && aiSettings.customEndpoints.empty()) {
                migrateLegacyEndpoint(aiSettings, aiNode["custom_endpoint"].as<std::string>());
            }
            if (aiNode["approval_mode"]) aiSettings.approvalMode = ai::approvalModeFromString(aiNode["approval_mode"].as<std::string>());
            if (aiNode["request_timeout_seconds"]) aiSettings.requestTimeoutSeconds = aiNode["request_timeout_seconds"].as<int>();
            if (aiNode["max_output_tokens"]) aiSettings.maxOutputTokens = aiNode["max_output_tokens"].as<int>();
            if (aiNode["max_tool_rounds"]) aiSettings.maxToolRounds = aiNode["max_tool_rounds"].as<int>();
            if (aiSettings.model.empty()) {
                aiSettings.model = ai::defaultModelForProvider(aiSettings.provider);
            }
        }

        return true;
    } catch (const std::exception& e) {
        Out::error("Failed to load settings: %s", e.what());
        return false;
    }
}

bool AppSettings::saveSettings() {
    try {
        // Update settings data
        
        // Project settings
        if (!lastProjectPath.empty() && std::filesystem::exists(lastProjectPath)) {
            settingsData["last_project_path"] = lastProjectPath.string();
        } else {
            settingsData.remove("last_project_path");
        }
        
        YAML::Node recentProjectsNode;
        for (const auto& path : recentProjects) {
            recentProjectsNode.push_back(path.string());
        }
        settingsData["recent_projects"] = recentProjectsNode;

        // Last compiler kit
        if (!lastCMakeCCompiler.empty() || !lastCMakeCxxCompiler.empty() || !lastCMakeGenerator.empty()) {
            YAML::Node kitNode;
            kitNode["c_compiler"] = lastCMakeCCompiler;
            kitNode["cxx_compiler"] = lastCMakeCxxCompiler;
            kitNode["generator"] = lastCMakeGenerator;
            settingsData["cmake_kit"] = kitNode;
        } else {
            settingsData.remove("cmake_kit");
        }

        // Emscripten SDK path override
        if (!emsdkPath.empty()) {
            YAML::Node emsdkNode;
            emsdkNode["path"] = emsdkPath;
            settingsData["emsdk"] = emsdkNode;
        } else {
            settingsData.remove("emsdk");
        }

        // cmake executable override
        if (!cmakePath.empty()) {
            YAML::Node cmakeNode;
            cmakeNode["path"] = cmakePath;
            settingsData["cmake"] = cmakeNode;
        } else {
            settingsData.remove("cmake");
        }

        if (!defaultExportDirectory.empty()) {
            YAML::Node exportNode;
            exportNode["default_dir"] = defaultExportDirectory.string();
            settingsData["export"] = exportNode;
        } else {
            settingsData.remove("export");
        }
        
        // Window settings
        YAML::Node windowNode;
        windowNode["width"] = windowWidth;
        windowNode["height"] = windowHeight;
        windowNode["maximized"] = isMaximized;
        // Omitted at 0 so "never recorded" stays distinguishable from a scale.
        if (windowUiScale > 0.0f) {
            windowNode["ui_scale"] = windowUiScale;
        }
        if (layoutUiScale > 0.0f) {
            windowNode["layout_ui_scale"] = layoutUiScale;
        }
        settingsData["window"] = windowNode;
        
        // Resources window settings
        YAML::Node resNode;
        resNode["icon_size"] = resourcesIconSize;
        resNode["layout"] = resourcesLayout;
        resNode["item_view_style"] = resourcesItemViewStyle;
        resNode["left_panel_width"] = resourcesLeftPanelWidth;
        settingsData["resources_window"] = resNode;

        // Code editor settings
        YAML::Node codeNode;
        codeNode["font_size"] = codeEditorFontSize;
        settingsData["code_editor"] = codeNode;

        // Editor viewport settings
        YAML::Node editorNode;
        editorNode["multi_viewport"] = multiViewportEnabled;
        editorNode["vsync"] = editorVSyncEnabled;
        YAML::Node panelsNode;
        panelsNode["structure"] = panelVisibility.structure;
        panelsNode["properties"] = panelVisibility.properties;
        panelsNode["resources"] = panelVisibility.resources;
        panelsNode["output"] = panelVisibility.output;
        panelsNode["animation"] = panelVisibility.animation;
        panelsNode["terrain"] = panelVisibility.terrain;
        panelsNode["ai_chat"] = panelVisibility.aiChat;
        editorNode["panels"] = panelsNode;
        settingsData["editor"] = editorNode;

        // AI assistant settings. Secrets must never be serialized here.
        YAML::Node aiNode;
        aiNode["provider"] = ai::toString(aiSettings.provider);
        aiNode["model"] = aiSettings.model;
        aiNode["endpoint_id"] = aiSettings.endpointId;
        YAML::Node endpointsNode(YAML::NodeType::Sequence);
        for (const ai::CustomEndpoint& endpoint : aiSettings.customEndpoints) {
            YAML::Node endpointNode;
            endpointNode["id"] = endpoint.id;
            endpointNode["label"] = endpoint.label;
            endpointNode["url"] = endpoint.url;
            endpointsNode.push_back(endpointNode);
        }
        aiNode["custom_endpoints"] = endpointsNode;
        aiNode["approval_mode"] = ai::toString(aiSettings.approvalMode);
        aiNode["request_timeout_seconds"] = aiSettings.requestTimeoutSeconds;
        aiNode["max_output_tokens"] = aiSettings.maxOutputTokens;
        aiNode["max_tool_rounds"] = aiSettings.maxToolRounds;
        settingsData["ai_assistant"] = aiNode;

        // Save to file
        std::ofstream fout(configFilePath.string());
        if (!fout) {
            Out::error("Failed to open editor settings for writing: %s", configFilePath.string().c_str());
            return false;
        }
        fout << YAML::Dump(settingsData);
        fout.close();
        if (!fout) {
            Out::error("Failed to write editor settings: %s", configFilePath.string().c_str());
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        Out::error("Failed to save settings: %s", e.what());
        return false;
    }
}

std::filesystem::path AppSettings::getLastProjectPath() {
    return lastProjectPath;
}

void AppSettings::setLastProjectPath(const std::filesystem::path& path) {
    lastProjectPath = path;
    // Also add to recent projects
    addToRecentProjects(path, false);

    saveSettings();
}

std::string AppSettings::getLastCMakeCCompiler() {
    return lastCMakeCCompiler;
}

std::string AppSettings::getLastCMakeCxxCompiler() {
    return lastCMakeCxxCompiler;
}

std::string AppSettings::getLastCMakeGenerator() {
    return lastCMakeGenerator;
}

void AppSettings::setLastCMakeKit(const std::string& cCompiler, const std::string& cxxCompiler, const std::string& generator) {
    lastCMakeCCompiler = cCompiler;
    lastCMakeCxxCompiler = cxxCompiler;
    lastCMakeGenerator = generator;
    saveSettings();
}

std::string AppSettings::getEmsdkPath() {
    return emsdkPath;
}

void AppSettings::setEmsdkPath(const std::string& path) {
    emsdkPath = path;
    saveSettings();
}

std::string AppSettings::getCMakePath() {
    return cmakePath;
}

void AppSettings::setCMakePath(const std::string& path) {
    cmakePath = path;
    saveSettings();
}

std::filesystem::path AppSettings::getDefaultExportDirectory() {
    return defaultExportDirectory;
}

void AppSettings::setDefaultExportDirectory(const std::filesystem::path& path) {
    defaultExportDirectory = path;
    saveSettings();
}

std::vector<std::filesystem::path> AppSettings::getRecentProjects() {
    return recentProjects;
}

void AppSettings::addToRecentProjects(const std::filesystem::path& path, bool needSave) {
    if (path.empty() || !std::filesystem::exists(path)) {
        return;
    }
    
    // Check if path already exists
    auto it = std::find_if(recentProjects.begin(), recentProjects.end(),
        [&path](const std::filesystem::path& p) {
            return p == path;
        });
    
    // If found, remove it (to add it to the front)
    if (it != recentProjects.end()) {
        recentProjects.erase(it);
    }
    
    // Add to the front
    recentProjects.insert(recentProjects.begin(), path);
    
    // Keep only the most recent 10 projects
    if (recentProjects.size() > 10) {
        recentProjects.resize(10);
    }
    
    // Save changes
    if (needSave)
        saveSettings();
}

void AppSettings::clearRecentProjects() {
    recentProjects.clear();
    saveSettings();
}

int AppSettings::getWindowWidth() {
    return windowWidth;
}

int AppSettings::getWindowHeight() {
    return windowHeight;
}

bool AppSettings::getIsMaximized() {
    return isMaximized;
}

void AppSettings::setWindowWidth(int width) {
    windowWidth = width;
}

void AppSettings::setWindowHeight(int height) {
    windowHeight = height;
}

void AppSettings::setIsMaximized(bool maximized) {
    isMaximized = maximized;
}

float AppSettings::getWindowUiScale() {
    return windowUiScale;
}

void AppSettings::setWindowUiScale(float scale) {
    windowUiScale = scale;
}

float AppSettings::getLayoutUiScale() {
    return layoutUiScale;
}

void AppSettings::setLayoutUiScale(float scale) {
    layoutUiScale = scale;
}

int AppSettings::getResourcesIconSize() {
    return resourcesIconSize;
}

int AppSettings::getResourcesLayout() {
    return resourcesLayout;
}

int AppSettings::getResourcesItemViewStyle() {
    return resourcesItemViewStyle;
}

float AppSettings::getResourcesLeftPanelWidth() {
    return resourcesLeftPanelWidth;
}

void AppSettings::setResourcesIconSize(int size) {
    resourcesIconSize = size;
}

void AppSettings::setResourcesLayout(int layout) {
    resourcesLayout = layout;
}

void AppSettings::setResourcesItemViewStyle(int style) {
    resourcesItemViewStyle = style;
}

void AppSettings::setResourcesLeftPanelWidth(float width) {
    resourcesLeftPanelWidth = width;
}

float AppSettings::getCodeEditorFontSize() {
    return codeEditorFontSize;
}

void AppSettings::setCodeEditorFontSize(float size) {
    codeEditorFontSize = std::clamp(size, minCodeEditorFontSize, maxCodeEditorFontSize);
}

bool AppSettings::getMultiViewportEnabled() {
    return multiViewportEnabled;
}

void AppSettings::setMultiViewportEnabled(bool enabled) {
    multiViewportEnabled = enabled;
}

bool AppSettings::getEditorVSyncEnabled() {
    return editorVSyncEnabled;
}

void AppSettings::setEditorVSyncEnabled(bool enabled) {
    editorVSyncEnabled = enabled;
}

PanelVisibilitySettings AppSettings::getPanelVisibility() {
    return panelVisibility;
}

void AppSettings::setPanelVisibility(const PanelVisibilitySettings& visibility) {
    panelVisibility = visibility;
    saveSettings();
}

ai::Settings AppSettings::getAiSettings() {
    return aiSettings;
}

void AppSettings::setAiSettings(const ai::Settings& settings) {
    aiSettings = settings;
    if (aiSettings.model.empty()) {
        aiSettings.model = ai::defaultModelForProvider(aiSettings.provider);
    }
    saveSettings();
}

} // namespace doriax::editor

namespace doriax::editor {

std::filesystem::path AppSettings::getExportTargetDir(const std::filesystem::path& projectFile, const std::string& mode) {
    const auto key = std::filesystem::absolute(projectFile).lexically_normal().generic_string();
    const YAML::Node data = settingsData;
    const auto exports = data["project_exports"];
    if (!exports || !exports.IsMap()) return {};
    const auto project = exports[key];
    if (!project || !project.IsMap()) return {};
    const auto entry = project[mode];
    if (!entry || !entry.IsMap()) return {};
    if (entry["targetDir"]) return entry["targetDir"].as<std::string>();
    return {};
}

bool AppSettings::setExportTargetDir(const std::filesystem::path& projectFile, const std::string& mode, const std::filesystem::path& targetDir) {
    if (getExportTargetDir(projectFile, mode) == targetDir) return true;
    const auto key = std::filesystem::absolute(projectFile).lexically_normal().generic_string();
    YAML::Node backup = YAML::Clone(settingsData);
    if (!targetDir.empty()) {
        YAML::Node entry(YAML::NodeType::Map);
        entry["targetDir"] = targetDir.generic_string();
        settingsData["project_exports"][key][mode] = entry;
    } else {
        settingsData["project_exports"][key].remove(mode);
        if (!settingsData["project_exports"][key].size()) settingsData["project_exports"].remove(key);
        if (!settingsData["project_exports"].size()) settingsData.remove("project_exports");
    }
    if (saveSettings()) return true;
    settingsData = backup;
    return false;
}

bool AppSettings::moveProjectLocalSettings(const std::filesystem::path& fromProjectFile, const std::filesystem::path& toProjectFile) {
    const auto fromKey = std::filesystem::absolute(fromProjectFile).lexically_normal().generic_string();
    const auto toKey = std::filesystem::absolute(toProjectFile).lexically_normal().generic_string();
    if (fromKey == toKey) return true;

    bool moved = false;
    for (const char* section : {"project_builds", "project_exports"}) {
        YAML::Node root = settingsData[section];
        if (!root || !root.IsMap() || !root[fromKey]) continue;

        root[toKey] = YAML::Clone(root[fromKey]);
        root.remove(fromKey);
        moved = true;
    }

    if (!moved) return true;

    // No rollback here: the project already moved, so restoring the old key would
    // point this session at settings it can no longer reach.
    return saveSettings();
}

LocalBuildSettings AppSettings::getBuildSettings(const std::filesystem::path& projectFile) {
    LocalBuildSettings result;
    const auto key = std::filesystem::absolute(projectFile).lexically_normal().generic_string();
    const auto builds = settingsData["project_builds"];
    if (!builds || !builds.IsMap()) return result;
    const auto entry = builds[key];
    if (!entry || !entry.IsMap()) return result;
    if (entry["c_compiler"]) result.cCompiler = entry["c_compiler"].as<std::string>();
    if (entry["cxx_compiler"]) result.cxxCompiler = entry["cxx_compiler"].as<std::string>();
    if (entry["generator"]) result.generator = entry["generator"].as<std::string>();
    if (entry["build_jobs"]) result.buildJobs = entry["build_jobs"].as<unsigned int>();
    result.configured = true;
    return result;
}

bool AppSettings::setBuildSettings(const std::filesystem::path& projectFile, const LocalBuildSettings& value) {
    const auto previous = getBuildSettings(projectFile);
    if (previous.configured && previous.cCompiler == value.cCompiler
        && previous.cxxCompiler == value.cxxCompiler && previous.generator == value.generator
        && previous.buildJobs == value.buildJobs) {
        return true;
    }

    const auto key = std::filesystem::absolute(projectFile).lexically_normal().generic_string();
    YAML::Node backup = YAML::Clone(settingsData);

    // Written even when empty: that entry pins the project to the default toolchain
    YAML::Node entry(YAML::NodeType::Map);
    if (!value.cCompiler.empty()) entry["c_compiler"] = value.cCompiler;
    if (!value.cxxCompiler.empty()) entry["cxx_compiler"] = value.cxxCompiler;
    if (!value.generator.empty()) entry["generator"] = value.generator;
    if (value.buildJobs) entry["build_jobs"] = value.buildJobs;
    settingsData["project_builds"][key] = entry;

    if (saveSettings()) return true;
    settingsData = backup;
    return false;
}

} // namespace doriax::editor
