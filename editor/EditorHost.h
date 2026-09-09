// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace doriax::editor {

    class CodeEditor;
    class ImageViewerWindow;
    class Properties;

    class EditorHost {
    public:
        virtual ~EditorHost() = default;

        virtual void enqueueMainThreadTask(std::function<void()> task);
        virtual void processMainThreadTasks();
        virtual bool isMainThread() const;

        virtual void addNewSceneToDock(uint32_t sceneId);
        virtual void clearSceneWindowState(uint32_t sceneId);
        virtual void prepareForProjectSwitch();
        virtual void resetLastActivatedScene();
        virtual bool shouldSyncEngineApi() const;
        virtual void updateResourcesPath();
        virtual void updateWindowTitle(const std::string& projectName);
        virtual void stopTransientPreviews();
        virtual void saveAllCodeEditors();
        virtual void requestScenePlayFocus(uint32_t sceneId);

        virtual void registerAlert(std::string title, std::string message);
        virtual void registerConfirmAlert(std::string title, std::string message, std::function<void()> onYes, std::function<void()> onNo = nullptr);
        virtual void registerThreeButtonAlert(std::string title, std::string message, std::function<void()> onYes, std::function<void()> onNo = nullptr, std::function<void()> onCancel = nullptr);
        virtual void registerSaveSceneDialog(uint32_t sceneId, std::function<void()> callback = nullptr);
        virtual void registerProjectSaveDialog(std::function<void()> callback = nullptr);

        virtual Properties* getPropertiesWindow() const;
        virtual CodeEditor* getCodeEditor() const;
        virtual ImageViewerWindow* getImageViewerWindow() const;
    };

    void setEditorHost(EditorHost* host);
    EditorHost& getEditorHost();

}
