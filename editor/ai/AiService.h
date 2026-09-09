// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"
#include "HttpClient.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

namespace doriax::editor::ai {

class EditorActionExecutor;

class AiService {
public:
    AiService();
    ~AiService();

    void setSettings(const Settings& settings);
    Settings getSettings() const;

    // Invoked from the worker when a reply lands, so the editor's ~100 ms idle wait
    // does not add a tick per agent step. Must be safe off the main thread.
    void setWakeCallback(std::function<void()> callback);

    // Cancels any in-flight request, drops the callback, joins the worker. Idempotent.
    void shutdown();

    // Attachments are consumed only when the message is accepted (returns
    // true); on a refusal (busy, empty input) the caller's vector is intact.
    bool sendUserMessage(const std::string& text,
                         std::vector<ChatAttachment>&& attachments = {});
    void cancel();
    bool isBusy() const;
    // Human-readable live state for the transcript. During rate-limit
    // backoff this includes the remaining delay and retry attempt.
    std::string getActivityText() const;

    // Drives the agent loop: reaps a finished worker and, when the conversation
    // ends on unanswered tool results, automatically sends a follow-up request
    // so the model can react to them. Call once per frame.
    void update();

    std::vector<ChatMessage> getMessages() const;
    // Bumped on every message-history mutation. Callers that need the messages
    // each frame cache the getMessages() snapshot and only re-copy when this
    // changes (messages can carry multi-MB attachments).
    uint64_t getRevision() const;
    std::vector<ActionProposal> getProposals() const;
    void clearConversation();

    // Replaces the in-memory conversation (e.g. when loading saved history).
    // Ignored while a request is in flight.
    void loadConversation(std::vector<ChatMessage> newMessages);

    ActionResult executeProposal(uint64_t proposalId, EditorActionExecutor& executor);
    void removeProposal(uint64_t proposalId);

    const HttpClient& getHttpClient() const { return httpClient; }

private:
    mutable std::mutex mutex;
    Settings settings;
    std::vector<ChatMessage> messages;
    std::vector<ActionProposal> proposals;
    std::thread worker;
    std::atomic<uint64_t> revision{1};
    std::atomic<bool> busy{false};
    std::atomic<bool> retryScheduled{false};
    std::atomic<bool> cancelRequested{false};
    uint64_t nextProposalId = 1;
    int toolRounds = 0;
    // Tracks one user-message agent turn (send → final assistant reply).
    bool turnActive = false;
    bool turnFailed = false;
    HttpClient httpClient;

    struct PendingRetry {
        ProviderRequest request;
        std::chrono::steady_clock::time_point readyAt;
        int attempt = 0;
    };
    std::optional<PendingRetry> pendingRetry;
    std::function<void()> wakeCallback;

    // Copies under the lock, invokes unlocked, so no wake runs holding the mutex.
    void notifyWake() const;
    std::string buildSystemPrompt() const;
    ProviderRequest buildRequestSnapshotLocked() const;
    void dispatchRequest(ProviderRequest request, int retryAttempt = 0);
    void runProviderRequest(ProviderRequest request, int retryAttempt);
    bool needsContinuationLocked() const;
    bool hasPendingProposalsLocked() const;
    void finishTurnLocked(bool success);
    void appendAssistantMessageLocked(const std::string& text);
    void addToolCallProposalLocked(const ToolCall& call);
    void dismissProposalLocked(ActionProposal& proposal, const std::string& note);
    void repairUnansweredToolCallsLocked();
};

} // namespace doriax::editor::ai
