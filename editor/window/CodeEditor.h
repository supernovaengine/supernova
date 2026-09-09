// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once
#include "imgui.h"
#include "widget/CustomTextEditor.h"
#include "Project.h"
#include "util/EntityPayload.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <thread>
#include <mutex>
#include <atomic>

namespace fs = std::filesystem;

namespace doriax::editor {

    struct EditorInstance {
        std::unique_ptr<CustomTextEditor> editor;
        bool isOpen;
        fs::path filepath;
        SyntaxLanguage languageType;
        fs::file_time_type lastWriteTime;
        bool isModified;
        double lastCheckTime;
        int savedUndoIndex;
        int propertyInsertUndoIndex; // undo index after a drag-drop insertion, -1 if none pending

        EditorInstance() : isOpen(true), languageType(SyntaxLanguage::None), isModified(false), lastCheckTime(0.0), savedUndoIndex(0), propertyInsertUndoIndex(-1) {}
    };

    class CodeEditor {
    private:
        struct PendingFileChange {
            fs::path filepath;
            fs::file_time_type newWriteTime;
        };

        Project* project;

        std::unordered_map<std::string, EditorInstance> editors;
        std::vector<PendingFileChange> changedFilesQueue;
        // Script files referenced by entities but not open here, project-relative path -> last seen mtime
        std::unordered_map<std::string, fs::file_time_type> watchedScriptFiles;
        double lastScriptWatchTime;
        bool isFileChangePopupOpen;
        bool windowFocused;
        EditorInstance* lastFocused;

        std::thread symbolParseThread;
        std::atomic<bool> isParsingSymbols{false};
        std::mutex parsedSymbolsMutex;
        std::vector<CustomTextEditor::ProjectSymbol> newLuaSymbols;
        std::vector<CustomTextEditor::ProjectSymbol> newCppSymbols;
        std::atomic<bool> newSymbolsReady{false};

        void checkFileChanges(EditorInstance& instance);
        // Drops the compiled forks of a shader source after it is written or reloaded
        void invalidateShadersForFile(const EditorInstance& instance);
        // Re-parses properties of script files edited outside the editor (e.g. in nano)
        void checkExternalScriptChanges();
        bool loadFileContent(EditorInstance& instance);
        void handleFileChangePopup();
        std::string getWindowTitle(const EditorInstance& instance) const;
        void updateScriptProperties(const EditorInstance& instance, const std::string& inMemoryContent = "");
        void updateScriptPropertiesForPath(const fs::path& relFilepath, const std::string& inMemoryContent = "");

        void updateAllProjectSymbols();
        void applyParsedProjectSymbols();

        fs::path resolveFilepath(const fs::path& relPath) const;
        std::string toRelativePath(const std::string& filepath) const;
        void insertLuaEntityProperty(EditorInstance& instance, Entity entity, uint32_t entitySceneId);
        void insertCppEntityProperty(EditorInstance& instance, Entity entity, uint32_t entitySceneId);

        static std::string toCamelCase(const std::string& name);
        static std::string toDisplayName(const std::string& camelCase);
        static void offsetToLineCol(const std::string& text, size_t offset, int& line, int& col);

        static void applyFontZoom(int delta);
        void showSettingsButton();

    public:
        CodeEditor(Project* project);
        ~CodeEditor();

        std::vector<fs::path> getOpenPaths() const;
        bool isFocused() const;
        void closeAll();

        bool save(const std::string& filepath);
        bool save(EditorInstance& instance);
        bool saveLastFocused();
        void saveAll();
        void undoLastFocused();
        void redoLastFocused();
        bool canUndoLastFocused() const;
        bool canRedoLastFocused() const;

        bool hasUnsavedChanges() const;
        bool hasLastFocusedUnsavedChanges() const;

        void openFile(const std::string& filepath, bool dockToCentral = false);
        void closeFile(const std::string& filepath);
        bool isFileOpen(const std::string& filepath) const;
        bool isFileModified(const std::string& filepath) const;
        void setText(const std::string& filepath, const std::string& text);
        std::string getText(const std::string& filepath) const;
        bool handleFileRename(const fs::path& oldPath, const fs::path& newPath);

        void show();
    };
}
