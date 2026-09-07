// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "ai/AiTypes.h"
#include "yaml-cpp/yaml.h"

namespace doriax::editor {

struct PanelVisibilitySettings {
    bool structure = true;
    bool properties = true;
    bool resources = true;
    bool output = true;
    bool animation = true;
    bool terrain = false;
    bool aiChat = true;

    bool operator==(const PanelVisibilitySettings& other) const {
        return structure == other.structure
            && properties == other.properties
            && resources == other.resources
            && output == other.output
            && animation == other.animation
            && terrain == other.terrain
            && aiChat == other.aiChat;
    }

    bool operator!=(const PanelVisibilitySettings& other) const {
        return !(*this == other);
    }
};

class AppSettings {
private:
    static std::filesystem::path configFilePath;
    static YAML::Node settingsData;
    
    // Recent projects list (max 10)
    static std::vector<std::filesystem::path> recentProjects;
    static std::filesystem::path lastProjectPath;

    // Last compiler kit chosen in Project Settings, so new (temp) projects
    // inherit it instead of silently defaulting each time.
    static std::string lastCMakeCCompiler;
    static std::string lastCMakeCxxCompiler;
    static std::string lastCMakeGenerator;

    // Emscripten SDK root override for Web exports ("" = auto-detect).
    // Machine-specific, so stored editor-wide rather than per-project.
    static std::string emsdkPath;

    // cmake executable override ("" = look it up on PATH). Machine-specific,
    // stored editor-wide like emsdkPath.
    static std::string cmakePath;
    static std::filesystem::path defaultExportDirectory;
    
    // Window settings. The size is physical pixels, only meaningful again at the
    // scale it was captured at; 0 marks a file from before that was tracked.
    static int windowWidth;
    static int windowHeight;
    static bool isMaximized;
    static float windowUiScale;

    // The dock layout is pixels too, but keeps the scale it was built at even if
    // the window later moves to a monitor that changes windowUiScale.
    static float layoutUiScale;

    // Resources window settings
    static int resourcesIconSize;
    static int resourcesLayout;      // 0=AUTO, 1=GRID, 2=SPLIT_FILES_ONLY, 3=SPLIT
    static int resourcesItemViewStyle; // 0=CARD, 1=CLASSIC
    static float resourcesLeftPanelWidth;

    // Code editor settings
    static float codeEditorFontSize;

    // Editor viewport settings
    static bool multiViewportEnabled;
    static bool editorVSyncEnabled;
    static PanelVisibilitySettings panelVisibility;

    // AI assistant settings (API keys are intentionally not stored here)
    static ai::Settings aiSettings;

    // Private methods
    static void ensureConfigDirectory();

public:
    // Initialization
    static bool initialize();

    // Directory where editor config files live (settings.yaml, layout ini, ...)
    static std::filesystem::path getConfigDirectory();

    // Project settings
    static std::filesystem::path getLastProjectPath();
    static void setLastProjectPath(const std::filesystem::path& path);
    
    static std::vector<std::filesystem::path> getRecentProjects();
    static void addToRecentProjects(const std::filesystem::path& path, bool needSave = true);
    static void clearRecentProjects();

    // Last compiler kit (empty strings = Default toolchain)
    static std::string getLastCMakeCCompiler();
    static std::string getLastCMakeCxxCompiler();
    static std::string getLastCMakeGenerator();
    static void setLastCMakeKit(const std::string& cCompiler, const std::string& cxxCompiler, const std::string& generator);

    // Emscripten SDK root override for Web exports ("" = auto-detect)
    static std::string getEmsdkPath();
    static void setEmsdkPath(const std::string& path);

    // cmake executable override ("" = look it up on PATH)
    static std::string getCMakePath();
    static void setCMakePath(const std::string& path);
    static std::filesystem::path getDefaultExportDirectory();
    static void setDefaultExportDirectory(const std::filesystem::path& path);
    
    // Window settings
    static int getWindowWidth();
    static int getWindowHeight();
    static bool getIsMaximized();
    
    static float getWindowUiScale();
    static float getLayoutUiScale();

    static void setWindowWidth(int width);
    static void setWindowHeight(int height);
    static void setIsMaximized(bool maximized);
    static void setWindowUiScale(float scale);
    static void setLayoutUiScale(float scale);
    
    // Resources window settings
    static int getResourcesIconSize();
    static int getResourcesLayout();
    static int getResourcesItemViewStyle();
    static float getResourcesLeftPanelWidth();
    
    static void setResourcesIconSize(int size);
    static void setResourcesLayout(int layout);
    static void setResourcesItemViewStyle(int style);
    static void setResourcesLeftPanelWidth(float width);

    // Code editor settings
    static constexpr float defaultCodeEditorFontSize = 14.0f; // VSCode default
    static constexpr float minCodeEditorFontSize = 8.0f;
    static constexpr float maxCodeEditorFontSize = 40.0f;

    static float getCodeEditorFontSize();
    static void setCodeEditorFontSize(float size);

    // Multi-viewport: allow dockable windows to be dragged out into their own OS window
    static bool getMultiViewportEnabled();
    static void setMultiViewportEnabled(bool enabled);

    // VSync for the editor's own frames; Play mode and builds use the project setting
    static bool getEditorVSyncEnabled();
    static void setEditorVSyncEnabled(bool enabled);

    // Panels toggled from the View menu
    static PanelVisibilitySettings getPanelVisibility();
    static void setPanelVisibility(const PanelVisibilitySettings& visibility);

    // AI assistant settings
    static ai::Settings getAiSettings();
    static void setAiSettings(const ai::Settings& settings);

    // Load and save settings
    static bool loadSettings();
    static bool saveSettings();
};

} // namespace doriax::editor
