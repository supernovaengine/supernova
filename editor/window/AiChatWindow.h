// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Project.h"
#include "ai/AiService.h"
#include "ai/ConversationStore.h"
#include "ai/ModelCatalog.h"
#include "window/dialog/AiSettingsWindow.h"
#include "window/widget/SelectableTextView.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace doriax::editor {

class ResourcesWindow;

// Clean, Copilot-style chat surface. Provider/key details live in
// AiSettingsWindow; quick conversation controls live with the composer.
class AiChatWindow {
private:
    enum class MentionKind {
        Selection,
        Project,
        Scene,
        Entity,
        File,
        OpenFile
    };

    struct MentionItem {
        MentionKind kind = MentionKind::File;
        std::string label;
        std::string insertText;
        std::string detail;
        int score = 0;
    };

    struct MentionState {
        bool open = false;
        int atRawPos = -1;          // raw-buffer index of the triggering '@'
        int queryRawEnd = -1;       // raw-buffer cursor while filtering
        int selectedIndex = 0;
        bool scrollToSelected = false;
        std::string query;
        std::vector<MentionItem> items;
        ImVec2 popupPos{};
        float inputItemMinY = 0.0f;
        float inputItemMaxY = 0.0f;
        float maxLineWidth = 0.0f;
    };

    Project* project;
    ResourcesWindow* resourcesWindow;
    ai::AiService service;
    ai::ConversationStore conversationStore;
    ai::ModelCatalog modelCatalog;
    AiSettingsWindow settingsWindow;
    SelectableTextView transcript;
    MentionState mention;
    int mentionDisplayCursor = 0;
    ImGuiID mentionInputId = 0;
    int pendingMentionRawCursor = -1; // raw caret after @ insert (stable across soft-wrap)

    bool windowOpen = true;
    bool focusRequested = false;
    bool windowFocused = false;
    bool isWindowVisible = false;
    bool hasNotification = false;
    bool syncedInitialSettings = false;
    bool scrollToBottom = false;
    bool refocusInput = false;
    bool customModelPopupRequested = false;
    // Account the custom-model popup applies to, captured when it opens.
    ai::ProviderId customModelProvider = ai::ProviderId::OpenAI;
    std::string customModelEndpointId;
    std::array<char, 8192> inputBuffer{};
    std::array<char, 256> customModelBuffer{};
    std::vector<int> inputSoftWraps;
    std::string inputDisplayText;
    std::vector<ai::ChatAttachment> pendingAttachments;
    // Snapshot of the service history, re-copied only when the service
    // revision changes: messages can carry multi-MB attachments, so copying
    // them every frame is too expensive.
    std::vector<ai::ChatMessage> messagesCache;
    uint64_t messagesCacheRevision = 0;
    // True while a Ctrl+V consumed as an attachment paste is still held, so
    // key-repeat frames keep suppressing the InputText's own text paste.
    bool attachmentPasteHeld = false;
    std::string currentConversationId;
    // Snapshot of saved conversations, refreshed when the history popup opens
    // (and after a delete) so list() doesn't re-parse every file per frame.
    std::vector<ai::ConversationMeta> historyCache;
    std::string pendingDeleteId; // armed delete awaiting a confirming second click
    std::string autoLoadedProjectPath;
    size_t lastSavedMessageCount = 0;
    size_t lastObservedMessageCount = 0;
    bool autoLoadedLatestConversation = false;

    void drawHeader();
    void drawHistoryPopup();
    void drawTranscript(float height);
    void drawProposals(const std::vector<ai::ActionProposal>& proposals, float height);
    void drawComposer(float inputHeight);
    void drawPendingAttachments();
    void drawComposerControls();
    void drawEditableSettingLabel(const std::string& value, float width,
                                  const char* editId, const char* popupId,
                                  const ImVec4* textColor = nullptr);
    void drawModelPopup();
    void drawCustomModelPopup();
    void drawApprovalPopup();
    void drawMentionPopup();
    void drawPromptMentionOverlay(ImVec2 inputMin, ImVec2 inputMax, ImGuiID inputId);
    void handlePromptDrop(float maxLineWidth);
    bool handleAttachmentPaste(ImGuiID inputId);
    void attachExternalFiles(const std::vector<std::string>& paths);
    bool attachExternalFile(const std::filesystem::path& path);
    bool attachImageBytes(std::string name, std::string mimeType,
                          std::vector<unsigned char> data);
    bool attachImageBase64(std::string name, std::string mimeType,
                           const std::string& base64Data);
    bool attachmentAlreadyPending(const std::string& name, const std::string& data) const;
    bool attachmentBudgetAllows(size_t newRawBytes) const;
    const std::vector<ai::ChatMessage>& cachedMessages();
    void closeMentionPopup();
    void refreshMentionItems();
    void collectMentionCandidates(std::vector<MentionItem>& out) const;
    bool applySelectedMention();
    bool insertDroppedMentions(const std::vector<std::string>& mentionBodies,
                               float maxLineWidth);
    bool replacePromptRange(int rawStart, int rawEnd, const std::string& replacement,
                            float maxLineWidth);
    bool handleMentionKeys();
    void updateMentionFromInput(int displayCursor, float inputMinX, float inputMinY,
                                float inputMaxY, float framePaddingX, float maxLineWidth);
    static const char* mentionKindIcon(MentionKind kind);
    static const char* mentionKindLabel(MentionKind kind);
    static int mentionKindBoost(MentionKind kind);
    void applySettings(const ai::Settings& settings);
    bool canSendMessage(std::string* reason = nullptr) const;
    void submitMessage();
    void executeProposal(uint64_t proposalId);
    void autoRunProposals();
    void updateMessageNotification();
    void loadLatestConversationForCurrentProject();
    void persistConversation();
    void loadConversation(const std::string& id);
    void prefetchModels();

public:
    static constexpr const char* WINDOW_NAME = "AI Chat";

    AiChatWindow(Project* project, ResourcesWindow* resourcesWindow);

    // Wakes the backend loop from the AI worker instead of waiting out its idle tick.
    void setWakeCallback(std::function<void()> callback);

    // Stops the AI worker: this window is never deleted, so ~AiService never runs.
    void shutdown();

    // Advances the agent loop (tool continuations, auto-run, persistence).
    // Safe to call every frame even when the panel is closed, collapsed, or
    // the OS window is minimized — the HTTP worker is independent of focus.
    void update();

    void show();
    void setOpen(bool open);
    bool isOpen() const;
    bool isFocused() const;

    // Clears the in-memory chat. Used by the header New Chat button and when
    // the editor starts a fresh project at the same path (e.g. File > New Project).
    void startNewChat();

    // Used by the InputText history callback to steer Up/Down into the @ picker.
    bool handleMentionHistoryKey(ImGuiKey key);
};

} // namespace doriax::editor
