// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "AiService.h"

#include "AiProvider.h"
#include "EditorActionExecutor.h"
#include "EditorActionRegistry.h"
#include "Out.h"
#include "RateLimitRetry.h"
#include "SecretStore.h"
#include "util/CxxStandards.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <deque>
#include <memory>
#include <random>
#include <sstream>
#include <unordered_map>

namespace doriax::editor::ai {

namespace {

constexpr int kMaxRateLimitRetries = 5;
constexpr auto kRetryTimingMargin = std::chrono::milliseconds(250);
constexpr auto kMaxProviderRetryDelay = std::chrono::minutes(5);
constexpr size_t kFullHistoricalToolRounds = 1;
constexpr size_t kHistoricalToolResultBytes = 2048;
constexpr size_t kHistoricalArgumentStringBytes = 512;

struct ToolRoundRange {
    size_t assistant = 0;
    size_t toolBegin = 0;
    size_t toolEnd = 0;
};

size_t utf8PrefixLength(const std::string& value, size_t maxBytes) {
    size_t end = std::min(value.size(), maxBytes);
    if (end == value.size()) {
        return end;
    }
    while (end > 0 &&
           (static_cast<unsigned char>(value[end]) & 0xc0u) == 0x80u) {
        --end;
    }
    return end;
}

std::string compactHistoricalToolContent(const std::string& content) {
    if (content.size() <= kHistoricalToolResultBytes) {
        return content;
    }

    const size_t prefixBytes = utf8PrefixLength(
        content, kHistoricalToolResultBytes);
    return content.substr(0, prefixBytes) +
           "\n... [older tool result compacted; " +
           std::to_string(content.size() - prefixBytes) + " bytes omitted]";
}

void compactHistoricalArgument(Json& value) {
    if (value.is_string()) {
        const std::string text = value.get<std::string>();
        if (text.size() > kHistoricalArgumentStringBytes) {
            const size_t prefixBytes = utf8PrefixLength(
                text, kHistoricalArgumentStringBytes);
            value = text.substr(0, prefixBytes) +
                    "\n... [older tool argument compacted]";
        }
        return;
    }

    if (value.is_array()) {
        for (Json& item : value) {
            compactHistoricalArgument(item);
        }
        return;
    }

    if (value.is_object()) {
        for (auto& item : value.items()) {
            compactHistoricalArgument(item.value());
        }
    }
}

// Provider requests are stateless, so their history must retain valid
// assistant/tool pairings. Once a later model response has consumed a tool
// round, however, its large source dumps and write arguments can be shortened.
// The stored conversation is never changed, and the newest completed round
// plus the currently unanswered round remain verbatim as working context.
void compactCompletedToolHistory(std::vector<ChatMessage>& messages) {
    std::vector<ToolRoundRange> completedRounds;
    for (size_t i = 0; i < messages.size(); ++i) {
        if (messages[i].role != ChatRole::Assistant ||
            messages[i].toolCalls.empty()) {
            continue;
        }

        const size_t toolBegin = i + 1;
        size_t toolEnd = toolBegin;
        while (toolEnd < messages.size() &&
               messages[toolEnd].role == ChatRole::Tool) {
            ++toolEnd;
        }

        // A later non-tool message means the provider already consumed this
        // result run. A trailing tool run is the continuation being built now.
        if (toolEnd > toolBegin && toolEnd < messages.size()) {
            completedRounds.push_back({i, toolBegin, toolEnd});
        }
        if (toolEnd > toolBegin) {
            i = toolEnd - 1;
        }
    }

    if (completedRounds.size() <= kFullHistoricalToolRounds) {
        return;
    }

    const size_t compactCount =
        completedRounds.size() - kFullHistoricalToolRounds;
    for (size_t roundIndex = 0; roundIndex < compactCount; ++roundIndex) {
        const ToolRoundRange& round = completedRounds[roundIndex];
        ChatMessage& assistant = messages[round.assistant];
        const bool hasSignedReasoning =
            !assistant.thinkingBlocks.empty() ||
            std::any_of(
                assistant.toolCalls.begin(), assistant.toolCalls.end(),
                [](const ToolCall& call) {
                    return !call.thoughtSignature.empty();
                });
        if (!hasSignedReasoning) {
            for (ToolCall& call : assistant.toolCalls) {
                compactHistoricalArgument(call.arguments);
            }
        }
        for (size_t i = round.toolBegin; i < round.toolEnd; ++i) {
            messages[i].content =
                compactHistoricalToolContent(messages[i].content);
        }
    }
}

std::chrono::milliseconds retryDelay(const HttpResponse& response,
                                     const ProviderResponse& error,
                                     int nextAttempt) {
    std::chrono::milliseconds delay = rateLimitRetryDelay(
        response, error.error, error.retryDelay);
    if (delay.count() > 0) {
        return std::min(delay + kRetryTimingMargin,
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            kMaxProviderRetryDelay));
    }

    // No provider hint: use bounded exponential backoff with a small jitter so
    // multiple editor instances do not retry in lockstep.
    const int exponent = std::clamp(nextAttempt - 1, 0, 5);
    const auto base = std::chrono::seconds(1 << exponent);
    thread_local std::mt19937 rng(std::random_device{}());
    const auto jitter = std::chrono::milliseconds(
        std::uniform_int_distribution<int>(0, 500)(rng));
    return base + jitter;
}

// Turns a provider HTTP failure into a short, readable line. The detail is the
// provider's own error.message (already extracted by the response parsers),
// not the raw JSON envelope.
std::string humanizeProviderError(long status, const std::string& detail) {
    std::string label;
    bool recognized = true;
    if (status == 400) label = "The provider rejected the request";
    else if (status == 401 || status == 403) label = "Authentication failed - check your API key";
    else if (status == 404) label = "Model or endpoint not found - check your AI settings";
    else if (status == 429) label = "Rate limit or quota exceeded";
    else if (status >= 500) label = "The provider is unavailable, try again later";
    else {
        label = "Request failed";
        recognized = false;
    }

    // When the provider sent its own explanation, that alone reads best;
    // the status code only adds value for statuses we can't name.
    if (!detail.empty()) {
        return label + ". " + detail;
    }
    if (!recognized) {
        return label + " (HTTP " + std::to_string(status) + ").";
    }
    return label + ".";
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool looksLikeOllamaEndpoint(const Settings& settings) {
    if (settings.provider != ProviderId::OpenAICompatible) {
        return false;
    }
    std::string endpoint = lowercase(activeEndpointUrl(settings));
    return endpoint.find("ollama") != std::string::npos ||
           endpoint.find(":11434") != std::string::npos;
}

std::string pluralTokens(int count) {
    return count == 1 ? "token" : "tokens";
}

bool hasTokenUsage(const ProviderResponse& response) {
    return response.promptTokens >= 0 ||
           response.completionTokens >= 0 ||
           response.totalTokens >= 0;
}

bool promptUsedNearlyWholeContext(const ProviderResponse& response) {
    if (response.promptTokens < 0 || response.totalTokens <= 0) {
        return false;
    }
    int remaining = response.totalTokens - response.promptTokens;
    int smallHeadroom = std::max(32, response.totalTokens / 20);
    return remaining >= 0 && remaining <= smallHeadroom;
}

std::string tokenUsageSummary(const ProviderResponse& response) {
    std::ostringstream out;
    bool wrote = false;
    if (response.promptTokens >= 0) {
        out << response.promptTokens << " prompt";
        wrote = true;
    }
    if (response.completionTokens >= 0) {
        if (wrote) out << " + ";
        out << response.completionTokens << " output";
        wrote = true;
    }
    if (response.totalTokens >= 0) {
        if (wrote) out << " = ";
        out << response.totalTokens << " total";
    }
    return out.str();
}

// A context window comfortably above what this request already consumed, rounded
// up to a tidy boundary, so the suggestion leaves room for both prompt and reply.
int suggestContextTokens(const ProviderResponse& response) {
    int used = std::max(response.totalTokens, response.promptTokens);
    int target = used > 0 ? used * 2 : 8192;
    for (int boundary : {8192, 16384, 32768, 65536, 131072}) {
        if (target <= boundary) return boundary;
    }
    return target;
}

// Advice for a token limit that is the provider/model context window rather than
// Doriax's output cap. Ollama gets a concrete num_ctx suggestion because a small
// context window is a common culprit.
std::string contextRemedy(const ProviderRequest& request, const ProviderResponse& response) {
    if (!looksLikeOllamaEndpoint(request.settings)) {
        return "Use a larger-context model or reduce the conversation/project context and try again.";
    }
    int suggested = suggestContextTokens(response);
    std::ostringstream out;
    out << "For Ollama, raise the model context length to at least " << suggested
        << " (\"/set parameter num_ctx " << suggested << "\" in the Ollama session, or "
           "\"PARAMETER num_ctx " << suggested << "\" in the Modelfile)";
    if (response.totalTokens > 0) {
        out << "; this response stopped after " << response.totalTokens << " total tokens";
    }
    out << ". You can also use a larger-context model or reduce the conversation/project context, then try again.";
    return out.str();
}

std::string buildTruncationMessage(const ProviderRequest& request, const ProviderResponse& response) {
    const bool hasCompletionUsage = response.completionTokens >= 0;
    // The model reached (within rounding of) Doriax's own output cap, so the stop
    // is the editor's "Max output tokens" setting, not a provider/context limit.
    const bool reachedDoriaxCap =
        hasCompletionUsage && response.completionTokens + 1 >= request.settings.maxOutputTokens;
    const bool stoppedBeforeDoriaxCap = hasCompletionUsage && !reachedDoriaxCap;
    // Only trust the "context full" reading when the stop was not Doriax's cap;
    // otherwise a small cap plus a largish prompt can look like a full window.
    const bool contextLikelyFull = promptUsedNearlyWholeContext(response) && !reachedDoriaxCap;

    std::ostringstream out;
    if (contextLikelyFull) {
        out << "The provider stopped at a token limit";
        if (!response.stopReason.empty()) {
            out << " (" << response.stopReason << ")";
        }
        if (response.completionTokens >= 0) {
            out << " after " << response.completionTokens << " output "
                << pluralTokens(response.completionTokens);
        }
        out << " because the prompt used nearly the whole context window";
        if (hasTokenUsage(response)) {
            out << " (" << tokenUsageSummary(response) << " tokens)";
        }
        out << ". This is not Doriax's \"Max output tokens\" cap ("
            << request.settings.maxOutputTokens << "). " << contextRemedy(request, response);
        return out.str();
    }

    if (stoppedBeforeDoriaxCap) {
        out << "The provider stopped at its own token limit";
        if (!response.stopReason.empty()) {
            out << " (" << response.stopReason << ")";
        }
        out << " after " << response.completionTokens << " output "
            << pluralTokens(response.completionTokens)
            << ", before Doriax's configured max of "
            << request.settings.maxOutputTokens << ".";
        if (hasTokenUsage(response)) {
            out << " Usage: " << tokenUsageSummary(response) << " tokens.";
        }
        out << " " << contextRemedy(request, response);
        return out.str();
    }

    out << "The response hit the max output token limit ("
        << request.settings.maxOutputTokens
        << ") and was cut off before completing.";
    if (hasTokenUsage(response)) {
        out << " Usage: " << tokenUsageSummary(response) << " tokens.";
    }
    out << " Increase \"Max output tokens\" in AI settings (gear icon) and try again - "
           "writing a whole file such as a shader needs a higher limit.";
    return out.str();
}

} // namespace

AiService::AiService() = default;

AiService::~AiService() {
    cancel();
    if (worker.joinable()) {
        worker.join();
    }
}

void AiService::setSettings(const Settings& newSettings) {
    std::lock_guard<std::mutex> lock(mutex);
    settings = newSettings;
    settings.requestTimeoutSeconds = std::clamp(settings.requestTimeoutSeconds, 1, 3600);
    settings.maxOutputTokens = std::clamp(settings.maxOutputTokens, 256, 16000);
    settings.maxToolRounds = std::clamp(settings.maxToolRounds, 1, 100);
    if (settings.model.empty()) {
        settings.model = defaultModelForProvider(settings.provider);
    }
}

void AiService::setWakeCallback(std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    wakeCallback = std::move(callback);
}

void AiService::shutdown() {
    cancel();
    {
        // Dropped before the join, so a late reply cannot wake a torn-down target.
        std::lock_guard<std::mutex> lock(mutex);
        wakeCallback = nullptr;
    }
    // curl aborts from its progress callback, so this waits well under a second.
    if (worker.joinable()) {
        worker.join();
    }
}

void AiService::notifyWake() const {
    std::function<void()> callback;
    {
        std::lock_guard<std::mutex> lock(mutex);
        callback = wakeCallback;
    }
    if (callback) callback();
}

Settings AiService::getSettings() const {
    std::lock_guard<std::mutex> lock(mutex);
    return settings;
}

bool AiService::sendUserMessage(const std::string& text,
                                std::vector<ChatAttachment>&& attachments) {
    // No move has happened yet: refusing here leaves the caller's vector
    // intact, so pending attachments survive a rejected send.
    if ((text.empty() && attachments.empty()) || isBusy()) {
        return false;
    }

    ProviderRequest request;
    {
        std::lock_guard<std::mutex> lock(mutex);
        // Typing a new message instead of approving is an implicit decline.
        // Resolve pending tool calls before the user message goes into the
        // history: an assistant tool call left unanswered ahead of a user
        // message makes every provider reject the replayed conversation.
        for (auto& proposal : proposals) {
            if (!proposal.executed) {
                dismissProposalLocked(proposal,
                    "The user did not approve this action and sent a new message instead.");
            }
        }
        // Loaded conversations arrive without proposals, so unanswered tool
        // calls persisted in the history are repaired from the messages
        // themselves.
        repairUnansweredToolCallsLocked();
        ChatMessage userMessage;
        userMessage.role = ChatRole::User;
        userMessage.content = text;
        userMessage.model = settings.model;
        userMessage.attachments = std::move(attachments);
        messages.push_back(std::move(userMessage));
        ++revision;
        toolRounds = 0;
        turnActive = true;
        turnFailed = false;
        request = buildRequestSnapshotLocked();
    }
    dispatchRequest(std::move(request));
    return true;
}

void AiService::cancel() {
    cancelRequested.store(true);

    std::lock_guard<std::mutex> lock(mutex);
    if (pendingRetry) {
        pendingRetry.reset();
        retryScheduled.store(false);
        appendAssistantMessageLocked("Request cancelled.");
        turnFailed = true;
    }
}

bool AiService::isBusy() const {
    return busy.load() || retryScheduled.load();
}

std::string AiService::getActivityText() const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!pendingRetry) {
        // Show the in-flight tool step rather than an unchanging label.
        if (toolRounds > 0) {
            return "Thinking... (step " + std::to_string(toolRounds) + "/" +
                   std::to_string(std::max(1, settings.maxToolRounds)) + ")";
        }
        return "Thinking...";
    }

    const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
        pendingRetry->readyAt - std::chrono::steady_clock::now());
    const int64_t remainingMilliseconds = std::max<int64_t>(0, remaining.count());
    const int64_t remainingSeconds = (remainingMilliseconds + 999) / 1000;
    if (remainingSeconds == 0) {
        return "Rate limited - retrying now (" +
               std::to_string(pendingRetry->attempt) + "/" +
               std::to_string(kMaxRateLimitRetries) + ")";
    }
    return "Rate limited - retrying in " + std::to_string(remainingSeconds) +
           "s (" + std::to_string(pendingRetry->attempt) + "/" +
           std::to_string(kMaxRateLimitRetries) + ")";
}

void AiService::update() {
    if (busy.load()) {
        return;
    }
    // Reap the worker that produced the latest response before reusing it.
    if (worker.joinable()) {
        worker.join();
    }

    ProviderRequest request;
    int retryAttempt = 0;
    bool dispatch = false;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (pendingRetry) {
            if (std::chrono::steady_clock::now() < pendingRetry->readyAt) {
                return;
            }
            request = std::move(pendingRetry->request);
            retryAttempt = pendingRetry->attempt;
            pendingRetry.reset();
            retryScheduled.store(false);
            dispatch = true;
        } else {
            const int maxRounds = std::max(1, settings.maxToolRounds);
            if (!needsContinuationLocked() || toolRounds >= maxRounds) {
                if (needsContinuationLocked() && toolRounds >= maxRounds) {
                    appendAssistantMessageLocked(
                        "Reached the tool-step limit for this request. Send another message to continue.");
                    finishTurnLocked(false);
                } else if (turnActive && !hasPendingProposalsLocked()) {
                    // Agent turn settled (final reply, or waiting is over with no more work).
                    finishTurnLocked(!turnFailed);
                }
                return;
            }
            ++toolRounds;
            request = buildRequestSnapshotLocked();
            dispatch = true;
        }
    }
    if (dispatch) {
        dispatchRequest(std::move(request), retryAttempt);
    }
}

std::vector<ChatMessage> AiService::getMessages() const {
    std::lock_guard<std::mutex> lock(mutex);
    return messages;
}

uint64_t AiService::getRevision() const {
    return revision.load();
}

std::vector<ActionProposal> AiService::getProposals() const {
    std::lock_guard<std::mutex> lock(mutex);
    return proposals;
}

void AiService::clearConversation() {
    if (isBusy()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex);
    messages.clear();
    ++revision;
    proposals.clear();
    toolRounds = 0;
    turnActive = false;
    turnFailed = false;
}

void AiService::loadConversation(std::vector<ChatMessage> newMessages) {
    if (isBusy()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex);
    messages = std::move(newMessages);
    ++revision;
    proposals.clear();
    toolRounds = 0;
    turnActive = false;
    turnFailed = false;
}

ActionResult AiService::executeProposal(uint64_t proposalId, EditorActionExecutor& executor) {
    ActionProposal proposal;
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& item : proposals) {
            if (item.id == proposalId) {
                if (item.executed) {
                    return item.result;
                }
                item.executing = true;
                proposal = item;
                break;
            }
        }
    }

    if (proposal.id == 0) {
        return {false, "Action proposal not found.", Json::object()};
    }

    ActionResult result = executor.execute(proposal.toolName, proposal.arguments, &cancelRequested);

    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& item : proposals) {
            if (item.id == proposalId) {
                item.executing = false;
                item.executed = true;
                item.result = result;
                break;
            }
        }

        ChatMessage toolMessage;
        toolMessage.role = ChatRole::Tool;
        toolMessage.toolCallId = proposal.toolCallId;
        toolMessage.toolName = proposal.toolName;
        toolMessage.toolDescription = proposal.description;
        toolMessage.toolSuccess = result.success;
        toolMessage.content = result.success ? result.message : ("Error: " + result.message);
        if (!result.data.empty()) {
            toolMessage.content += "\n" + result.data.dump(2);
        }
        messages.push_back(toolMessage);
        ++revision;
    }

    if (!result.success) {
        switch (result.failureSeverity) {
            case ActionFailureSeverity::ExpectedMiss:
                break;
            case ActionFailureSeverity::Warning:
                Out::warning("AI action warning: %s", result.message.c_str());
                break;
            case ActionFailureSeverity::Error:
                Out::error("AI action failed: %s", result.message.c_str());
                break;
        }
    }

    return result;
}

void AiService::removeProposal(uint64_t proposalId) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& proposal : proposals) {
        if (proposal.id != proposalId || proposal.executed) {
            continue;
        }
        dismissProposalLocked(proposal, "The user dismissed this action.");
        break;
    }
}

// Ensures every assistant tool call is answered exactly once, by a tool
// result inside the contiguous run that follows its assistant message. Live
// turns keep this shape through the proposal list; this covers histories that
// arrive without proposals (loadConversation), where an unanswered call would
// otherwise make providers reject the replay forever. A matching result
// stranded later in the history (a stale proposal that was executed after the
// conversation moved on) is relocated into place rather than duplicated, any
// tool result left with no call to answer is dropped, and calls with no
// result anywhere get a synthetic "declined" one.
void AiService::repairUnansweredToolCallsLocked() {
    // Pass 1: a tool message is anchored when it is the first result for one
    // of its own assistant's calls, inside the run right after that assistant.
    std::vector<bool> anchored(messages.size(), false);
    for (size_t i = 0; i < messages.size(); ++i) {
        if (messages[i].role != ChatRole::Assistant || messages[i].toolCalls.empty()) {
            continue;
        }
        std::unordered_map<std::string, size_t> expected;
        for (const ToolCall& call : messages[i].toolCalls) {
            ++expected[call.id];
        }
        size_t j = i + 1;
        while (j < messages.size() && messages[j].role == ChatRole::Tool) {
            auto expectedIt = expected.find(messages[j].toolCallId);
            if (expectedIt != expected.end() && expectedIt->second > 0) {
                anchored[j] = true;
                --expectedIt->second;
            }
            ++j;
        }
        i = j - 1;
    }

    // Everything else is an orphan: queue every result per call id as a
    // candidate to relocate next to a matching call. Queues preserve the
    // multiplicity of legacy ID-less parallel calls that share a name.
    std::unordered_map<std::string, std::deque<ChatMessage>> late;
    for (size_t i = 0; i < messages.size(); ++i) {
        if (messages[i].role == ChatRole::Tool && !anchored[i]) {
            const std::string callId = messages[i].toolCallId;
            late[callId].push_back(std::move(messages[i]));
        }
    }

    // Pass 2: rebuild, appending relocated or synthetic results for calls the
    // anchored run leaves unanswered.
    std::vector<ChatMessage> repaired;
    repaired.reserve(messages.size());
    for (size_t i = 0; i < messages.size(); ++i) {
        if (messages[i].role == ChatRole::Tool && !anchored[i]) {
            continue;
        }
        repaired.push_back(std::move(messages[i]));
        if (repaired.back().role != ChatRole::Assistant || repaired.back().toolCalls.empty()) {
            continue;
        }
        const std::vector<ToolCall> calls = repaired.back().toolCalls;

        std::unordered_map<std::string, size_t> answered;
        size_t j = i + 1;
        while (j < messages.size() && messages[j].role == ChatRole::Tool) {
            if (anchored[j]) {
                ++answered[messages[j].toolCallId];
                repaired.push_back(std::move(messages[j]));
            }
            ++j;
        }
        for (const ToolCall& call : calls) {
            auto answeredIt = answered.find(call.id);
            if (answeredIt != answered.end() && answeredIt->second > 0) {
                --answeredIt->second;
                continue;
            }
            auto lateIt = late.find(call.id);
            if (lateIt != late.end() && !lateIt->second.empty()) {
                repaired.push_back(std::move(lateIt->second.front()));
                lateIt->second.pop_front();
                if (lateIt->second.empty()) {
                    late.erase(lateIt);
                }
                continue;
            }
            ChatMessage toolMessage;
            toolMessage.role = ChatRole::Tool;
            toolMessage.toolCallId = call.id;
            toolMessage.toolName = call.name;
            toolMessage.toolDescription = EditorActionRegistry::describe(call.name, call.arguments);
            toolMessage.toolSuccess = false;
            toolMessage.content = "This action was never run before the conversation moved on; treat it as declined.";
            repaired.push_back(std::move(toolMessage));
        }
        i = j - 1;
    }
    messages = std::move(repaired);
    ++revision;
}

// Resolve the call with a dismissal result rather than dropping it, so the
// assistant's tool call stays answered and the next request that replays the
// history remains well-formed.
void AiService::dismissProposalLocked(ActionProposal& proposal, const std::string& note) {
    proposal.executing = false;
    proposal.executed = true;
    proposal.result = {false, "Dismissed by the user.", Json::object()};

    ChatMessage toolMessage;
    toolMessage.role = ChatRole::Tool;
    toolMessage.toolCallId = proposal.toolCallId;
    toolMessage.toolName = proposal.toolName;
    toolMessage.toolDescription = proposal.description;
    toolMessage.toolSuccess = false;
    toolMessage.content = note;
    messages.push_back(toolMessage);
    ++revision;
}

std::string AiService::buildSystemPrompt() const {
    std::ostringstream prompt;
    prompt
        << "You are Doriax Editor Assistant, an AI helper embedded inside the Doriax game editor.\n"
        << "You answer questions and drive the editor through the provided tools. The engine layer is not aware of you.\n"
        << "Call tools only when they help the user's explicit goal, and never invent tool names.\n"
        << "Read-only tools inspect project context. Mutating, file-writing, download, and import tools may require user approval before they run.\n"
        << "Use read_resource_file before editing existing scripts or materials. Use inspect_scene for scene-level settings.\n"
        << "2D scenes have a dedicated lighting model: create_entity light_2d adds a Light2DComponent (radius/falloff point light) and occluder_2d adds an Occluder2DComponent (shadow caster). Light2D adds on top of the scene's 2D ambient (set_scene_property ambient_light_2d_color/ambient_light_2d_intensity), so dim the ambient to make 2D lights visible. Occluder2D shape AUTO_QUAD derives its outline from a sibling mesh; POLYGON uses its own points. Enable a Light2D's shadows property to cast from occluders. Shadow edge smoothness is per scene: shadows_quality (3D) and shadows_2d_quality (2D) take int_value 0=none/1=low/2=medium/3=high. These are separate from the 3D Light/global_illumination path.\n"
        << "For requested 3D physics or collisions on an existing visible model/mesh, inspect the entity and use add_body3d_shape. It adds Body3DComponent and its collision Shape3D to that SAME entity atomically; never create a separate body entity unless the user explicitly asks for one. A Body3DComponent without at least one shape does not collide. Use dynamic for player/rigid-body actors and static for floors, terrain, and level geometry; a dynamic character needs a static collision shape under it. Shape dimensions and local position are in mesh-local units and the engine multiplies them by the entity's transform scale on its own, so NEVER pre-divide or pre-multiply a size by the scale to compensate. Never guess a collider size: omit the size arguments so the shape is fitted to the mesh's measured bounds, or read mesh_bounds.local_size from inspect_entity and pass that. If the user says a collider is too big or too small, do not nudge numbers by feel; re-read mesh_bounds and set the shape to the measured local size. Growing a shape's height expands it equally up and down about its centre, so shift the shape position by half the added height to keep the bottom in place. For physics-based jumping, add the body and collision shape before editing the controller, use confirmed Body3D physics APIs, and never describe manually changing an entity transform as physics.\n"
        << "For requested 2D physics or collisions on an existing sprite, tilemap, or 2D mesh, inspect the entity and use add_body2d_shape. It adds Body2DComponent and its Shape2D to that SAME entity atomically; never create a separate body entity unless explicitly requested. A Body2DComponent without a shape does not collide. Use dynamic for actors and static for ground/platforms, and use the requested primitive or polygon/chain points in entity-local 2D units. As in 3D these are mesh-local and the engine multiplies them by the entity scale, so never pre-divide a size to compensate; read mesh_bounds.local_size from inspect_entity instead of guessing. Do not confuse Body2D with Body3D or claim transform-only movement is physics.\n"
        << "After a tool returns, use its result to decide the next step and report concise progress to the user.\n"
        << "Work efficiently within the tool-step budget: batch independent tool calls into a single step instead of one per step, and never repeat a search or read whose result you already have. Gather the API facts you need, then write the file.\n"
        << "Prefer project-relative paths. Never request arbitrary shell commands. Never ask for secrets in chat.\n"
        << "For external assets, use curated sources only and preserve license/author/source attribution.\n"
        << "For scripts and engine API code, the Doriax engine source under the editor's engine/ directory (read it with search_engine_source and read_engine_source) is the ONLY source of truth. Use ONLY classes, methods, properties, enums, macros, and constructor overloads you have confirmed exist in that source; if a symbol is not present there it does not exist in Doriax, so do not use it. Never invent APIs or carry over names, macros, or patterns from other engines or frameworks (e.g. Godot GDCLASS, Unreal GENERATED_BODY/UPROPERTY, Qt Q_OBJECT). search_engine_api is only a quick index into that same source; when a symbol is unfamiliar or you are unsure of its exact spelling or overloads, confirm it in the source before writing it (e.g. key codes are Input.KEY_* in Lua but D_KEY_* macros in C++, and Quaternion's axis-angle constructor takes the angle first: Quaternion(angle, axis)).\n"
        << "A C++ method existing on a class does NOT mean Lua can call it. LuaBridge binds many accessors as properties instead of methods, and calling the accessor from Lua fails at runtime with \"attempt to call a nil value (method 'x')\". search_engine_api marks this: kind 'Method' with a ':' in the detail (Body2D:getMass()) is Lua-callable, while kind 'CppMethod' with '::' in the detail (Object::getPosition(), Body3D::setLinearVelocity()) is C++ only and carries lua_callable=false plus a lua_note naming the property to use (object.position, body.linearVelocity). In Lua, read and assign those properties (self.sphere.position = p, body.linearVelocity = Vector3(0,0,0)); never translate a CppMethod into obj:getX()/obj:setX(). If a class has no bound property for what you need either, check the LuaBridge binding source under engine/core/script/binding/ before writing the call.\n"
        << "A .scene file only belongs to the project while project.yaml lists it; an unlisted one is not built, exported, opened, or usable as a child scene, so a scene file that arrived from outside the editor needs add_project_scene first.\n"
        << "A bundle is only built and registered when a scene instantiates it or when it is listed as standalone, so a bundle that exists only to be spawned from a script with BundleManager needs set_standalone_bundle before createBundle can find it by name. Every createBundle call creates its own instance root and returns it, so the same bundle can be spawned repeatedly; the optional third argument is only the entity the new root is parented to (a name resolved in that scene, an entity id of that scene, or an object carrying its own scene), never the root itself. destroyBundle(scene, root) removes what that spawn created and leaves the parent alone.\n"
        << "When you create a script for requested behavior, write the complete script with update_script_file instead of asking the user to edit it manually.\n"
        << "A ScriptComponent holds a list of script entries; inspect_component on ScriptComponent lists them with index, class name, and file paths. attach_script adds an entry, so use it only for a script the entity does not have yet: to rename or repoint an entry it already has use update_script_entry, and to detach one use remove_script_entry. rename_resource already repoints the attached entries at the renamed file, so after renaming a script only its class name is left to fix with update_script_entry.\n"
        << "A project can declare C++ script directories: get_project_summary reports them as script_dirs and set_project_script_dirs changes them. Each root is an include directory and every source under it is compiled without any ScriptComponent referencing it, so shared helper, utility, and library classes belong there. Create one with create_source_file, which writes a source or header without attaching it to an entity; create_script is only for a script an entity actually runs. A .cpp that is neither under a root nor included by an attached script is not compiled at all, so add the root instead of assuming it builds. The editor also only runs a C++ build when an enabled C++ script is attached to an entity somewhere in the project: Lua scripts never trigger one, so in a Lua-only project the helper sources are compiled first by an export, and a clean startup log is no proof they built.\n"
        << "The project C++ language standard is get_project_summary's cxx_standard (" << cxxStandardList() << ") and set_project_cxx_standard changes it. The next play or export builds with it, and an export compiles the engine with it too. Do not edit CMAKE_CXX_STANDARD in the generated CMakeLists.txt.\n"
        << "The project CMakeLists.txt is generated on every build and any edit to it is lost, so never edit it or ask the user to. Custom build settings -- extra include directories, linked libraries, compile definitions -- go in ProjectBuild.cmake at the project root: read the current one with read_resource_file and rewrite it whole with update_project_build_file. Attach settings to ${DORIAX_TARGET} rather than a literal target name (it differs between the editor and exported builds) and use ${DORIAX_SCRIPTS_DIR} as the root of script paths, e.g. target_include_directories(${DORIAX_TARGET} PRIVATE ${DORIAX_SCRIPTS_DIR}/Source/Public) and target_link_libraries(${DORIAX_TARGET} PRIVATE mylib).\n"
        << "After creating or editing ANY script (C++ or Lua), verify it before telling the user it is ready: start the scene with control_play_mode (action=start), then call read_output_log to inspect the editor output. A C++ compile error appears as a build failure with compiler messages; a Lua or C++ runtime error appears as a 'Script crash in scene ...' entry. If the scene is still building, read_output_log again. If you find errors, stop play with control_play_mode (action=stop), fix the script with update_script_file, and verify again -- repeat until the log is clean, then stop play. Do not claim the script works without reading the log.\n"
        << "Doriax Lua scripts are plain returned module tables: local Name = { properties = {} }; function Name:init() ... end; return Name.\n"
        << "Editor-exposed Lua properties go in the properties array as { name = \"speed\", displayName = \"Speed\", type = \"float\", default = 5.0 } and are read at runtime as self.speed.\n"
        << "Choose Lua update events by timing domain. Variable-frame logic such as UI, cameras, animation, and direct non-physics transform movement uses RegisterEngineEvent(self, \"onUpdate\") with Name:onUpdate(); scale continuous non-physics movement by Engine.deltatime. Physics-step logic uses RegisterEngineEvent(self, \"onFixedUpdate\") with Name:onFixedUpdate(). Any continuously applied body force or torque MUST run in onFixedUpdate, never onUpdate: each physics world accumulates force calls until its next fixed step, so calling them from onUpdate makes the effective force depend on render FPS. The two body types spell these differently and are not interchangeable: Body3D has applyForce(force) for a centre force plus applyForce(force, point) and applyTorque(torque), and has NO applyForceToCenter; Body2D has applyForceToCenter(force, wake) plus applyForce(force, point, wake) (no single-argument overload exists) and applyTorque(torque, wake). Prefer onFixedUpdate for velocity control and held-input polling that drives physics too. Do not multiply forces or torques by Engine.deltatime; the physics engine integrates them over the fixed step. Never move a physics body by writing its entity transform from onFixedUpdate (object.position, object:setPosition): the body is synced from the transform before the step and written back after it, so the write is silently discarded. Teleport with body.position / body.rotation (Body3D) or body.position / body.angle (Body2D), which write the simulation directly; entities with no body have no such restriction. One-shot impulses must be emitted once, not every held-input update. No manual Lua unregister is needed.\n"
        << "For component/UI events in Lua use the global RegisterEvent(self, eventObject, \"method\"), e.g. RegisterEvent(self, Button(self.scene, self.entity):getButtonComponent().onPress, \"onPress\"). RegisterEngineEvent/RegisterEvent are Lua globals (not bindings); find them with search_engine_api.\n"
        << "Lua script instances receive self.scene and self.entity; self.entity is a numeric Entity id, not an object. Never call self.entity:... or self.entity....\n"
        << "They do not use Dori.Script, on_start, get_entity, get_component/getComponent, or set_property/setProperty.\n"
        << "Do not put editor property paths such as submeshes[0].material.baseColorFactor inside Lua scripts; use runtime wrappers and APIs from search_engine_api instead.\n"
        << "For Mesh/Shape color at runtime use Shape(self.scene, self.entity) then setColor(1,0,0,1) or setColor(Vector4(1,0,0,1)); do not call methods on self.entity directly.\n"
        << "Doriax C++ scripts use flat quoted headers from .doriax/engine-api (e.g. \"Mesh.h\", \"ScriptBase.h\", \"Engine.h\"). Never use #include <core/...>.\n"
        << "For mesh/cube behavior use cpp_subclass inheriting Mesh (or Shape) and call setColor() on this; do not use ScriptBase as if it had mesh APIs or onInit().\n"
        << "cpp_script_class inherits ScriptBase for general logic. Use REGISTER_ENGINE_EVENT(onUpdate) for variable-frame logic and REGISTER_ENGINE_EVENT(onFixedUpdate) for physics-step logic, with the matching UNREGISTER_ENGINE_EVENT in the destructor. Continuous Body2D/Body3D forces and torques must run in onFixedUpdate (velocity control and physics-driving input polling belong there too); never multiply forces or torques by Engine::getDeltatime(). Never move a physics body by writing its entity transform from onFixedUpdate (Object::setPosition/setRotation): the body is synced from the transform before the step and written back after it, so the write is silently discarded. Teleport with Body3D::setPosition/setRotation or Body2D::setPosition/setAngle; entities with no body have no such restriction.\n"
        << "A dynamic body's mass is never authored directly, it comes from the shape and its density, and the two dimensions do NOT share a formula or a default. In 3D mass is volume * density with default density 1000 (kg/m3, water), so a radius-1 sphere weighs about 4189 kg and a radius-0.5 sphere about 524 kg. In 2D mass is area * density with default density 1, and 2D shape sizes are points scaled to metres by pointsToMeterScale2D (64 by default), so a 100x100 point box weighs about 2.4 kg. Never apply 3D reasoning or the 3D default to a Body2D. Size forces from F = mass * desired acceleration, reading the real mass rather than guessing: a few hundred newtons on a multi-tonne body looks like a broken script. Mass is getMass() in C++, but in Lua it is the property body.mass on Body3D and the method body:getMass() on Body2D, so confirm the spelling in the bindings. If the user reports that movement needs an implausibly large force or feels sluggish, lower the shape density (or shrink the shape) instead of inflating the force, and say what the body actually weighs. Body3D::setMass only writes the live Jolt body and is lost on reload, so persist mass by authoring density.\n"
        << "A Doriax script class needs no class-declaration or reflection macro (no D_OBJECT/GDCLASS/GENERATED_BODY/Q_OBJECT): just inherit the base class and declare a (Scene*, Entity) constructor. Prefer editing the class skeleton create_script generated rather than rewriting it.\n"
        << "For component/UI events (button press, click, scrollbar change) use REGISTER_COMPONENT_EVENT(Component, event, method) or shortcuts REGISTER_UI_EVENT/REGISTER_BUTTON_EVENT/REGISTER_SCROLLBAR_EVENT/REGISTER_PANEL_EVENT in the constructor, paired with UNREGISTER_* in the destructor. These macros and DPROPERTY are C++ script macros (not Lua bindings); find them with search_engine_api.\n"
        << "Cameras orient by target, not by rotation: CameraComponent.useTarget defaults to true and Camera::setTarget re-enables it, so while it is on the view is lookAt(position, target, up) and the entity's rotation is ignored. Call disableTarget() first if you want the rotation to drive the view (forward = rotation * Vector3(0, 0, -1)). For a third-person or orbit camera, place the camera around the target with spherical coordinates -- offset = (cos(pitch)*sin(yaw), sin(pitch), cos(pitch)*cos(yaw)); position = target + offset*distance -- then call setTarget(target), so a positive pitch means the camera sits ABOVE the target. Do not derive the position by subtracting a rotated forward vector (target - forward*distance): rotations are right-handed, so a positive pitch tilts the VIEW up and that formula silently puts the camera below the target.\n"
        << "Engine angle arguments are in the engine's default unit, which is DEGREES unless Engine::setUseDegrees(false) was called: Quaternion(angle, axis), Quaternion::fromEulerAngles, Camera::rotateView and similar all pass their angle through Angle::defaultToRad, so never assume they take radians. Convert explicitly with Angle::degToDefault/Angle::radToDefault, and note that std::sin/std::cos always want radians (Angle::defaultToRad).\n"
        << "To print or log from scripts use the Log class: C++ Log::print(\"pos %f %f %f\", p.x, p.y, p.z) with #include \"Log.h\" (Log::warn/Log::error for severity); Lua Log.print(\"pos \" .. tostring(p)). Engine has no log method; never use Engine::log, printf, std::cout, or bare print().\n"
        << "Custom shaders are per-component on Mesh/UI/Points/Lines/Sky via the customShader property. To customize, call fork_shader (it copies the built-in <type>.vert/.frag into a project-relative directory and sets customShader in one undoable step; directory defaults to shaders, and fork_includes optionally copies the engine includes into an editable includes/ folder beside the fork). Never set customShader by hand to files that do not exist; the value is a base path with no extension (vert=base.vert, frag=base.frag) or two explicit paths joined by '|' (\"a.vert|b.frag\") when the entry points have different names. Both a .vert and a .frag are always required. Clear customShader (empty string) to reset to the built-in shader.\n"
        << "The forked .vert/.frag are complete, compiling engine shaders with a strict structure: a #version line, #include \"includes/...\" (resolved against the engine library), #ifdef variant guards, uniform blocks, texture and sampler declarations, in/out varyings, and a specific fragment output variable. Editing them with write_shader_file REQUIRES first calling read_resource_file on the forked file, then making minimal targeted changes that PRESERVE the #version, #include lines, uniform blocks, ALL original texture and sampler declarations (uniform sampler2D/samplerCube/textureN/samplerN and their #ifdef guards), in/out declarations, and the existing output variable name. Keep every original texture/sampler declaration even if your new code does not sample it - the engine binds them by fixed slot, so removing or renaming one breaks the texture bindings. Likewise keep the original main() code that READS the in varyings (v_position, v_normal, v_uv1, v_color, v_lightProjPos, etc.): if you delete a varying's usage the GLSL compiler strips that fragment input and the shader fails to cross-compile with a vertex/fragment interface mismatch (\"vertex shader output X does not exist in fragment shader inputs\"). Prefer to keep the original main() body that consumes the varyings and layer your effect on the final color (e.g. compute the original color, then posterize/override it), rather than deleting the existing computations. Only change the shading math (usually near the end of main()). Never rewrite a shader from scratch, drop the #version/#include lines, or invent uniform, sampler, texture, or output names - that produces compile errors (e.g. missing precision qualifier / wrong #version) or broken bindings. A stylized custom shader (e.g. toon/cel) may legitimately replace the lighting model and stop sampling the shadow or IBL maps; that is allowed - keep their declarations (the engine harmlessly skips the now-unused bindings), but briefly tell the user when your shader changes lighting behavior such as no longer receiving shadows, so they can ask to keep it. If a custom shader fails to compile, the object falls back to the built-in shader and the error appears in the output window; read that error and the forked source, then fix minimally.\n";
    return prompt.str();
}

ProviderRequest AiService::buildRequestSnapshotLocked() const {
    ProviderRequest request;
    request.settings = settings;
    request.apiKey = SecretStore::getApiKey(accountKey(settings));
    request.messages = messages;
    compactCompletedToolHistory(request.messages);
    request.tools = EditorActionRegistry::tools();
    request.systemPrompt = buildSystemPrompt();
    return request;
}

void AiService::dispatchRequest(ProviderRequest request, int retryAttempt) {
    // dispatchRequest only runs when no request is in flight, but the previous
    // worker may have finished without being reaped yet (update() usually joins
    // it). Join it here before reassigning, otherwise the std::thread move-assign
    // onto a still-joinable handle would call std::terminate.
    if (worker.joinable()) {
        worker.join();
    }
    cancelRequested.store(false);
    busy.store(true);
    worker = std::thread(&AiService::runProviderRequest, this,
                         std::move(request), retryAttempt);
}

bool AiService::needsContinuationLocked() const {
    // Continue only when the conversation ends on tool results the model has
    // not yet responded to, and nothing is still awaiting user approval.
    if (messages.empty() || messages.back().role != ChatRole::Tool) {
        return false;
    }
    for (const auto& proposal : proposals) {
        if (!proposal.executed) {
            return false;
        }
    }
    return true;
}

bool AiService::hasPendingProposalsLocked() const {
    for (const auto& proposal : proposals) {
        if (!proposal.executed) {
            return true;
        }
    }
    return false;
}

void AiService::finishTurnLocked(bool success) {
    if (!turnActive) {
        return;
    }
    turnActive = false;
    turnFailed = false;
    if (success) {
        Out::success("AI chat session completed");
    }
}

void AiService::runProviderRequest(ProviderRequest request, int retryAttempt) {
    // Declared first so it destructs last: the woken frame must see `busy` cleared.
    struct WakeOnExit {
        const AiService* service;
        ~WakeOnExit() { service->notifyWake(); }
    } wakeOnExit{this};

    if (request.apiKey.empty() && accountRequiresApiKey(request.settings.provider)) {
        std::lock_guard<std::mutex> lock(mutex);
        appendAssistantMessageLocked("No API key set for " + providerLabel(request.settings.provider) +
                                     ". Open AI settings to add one before sending requests.");
        turnFailed = true;
        busy.store(false);
        return;
    }

    if (request.settings.provider == ProviderId::OpenAICompatible &&
        activeEndpointUrl(request.settings).empty()) {
        std::lock_guard<std::mutex> lock(mutex);
        appendAssistantMessageLocked("The selected endpoint has no URL. Open AI settings to set "
                                     "its Chat Completions URL, or pick another model.");
        turnFailed = true;
        busy.store(false);
        return;
    }

    ProviderResponse parsed;
    try {
        std::unique_ptr<Provider> provider = createProvider(request.settings.provider);
        HttpRequest httpRequest = provider->buildRequest(request);
        httpRequest.timeoutSeconds = request.settings.requestTimeoutSeconds;
        HttpResponse httpResponse = httpClient.send(httpRequest, &cancelRequested);

        if (cancelRequested.load()) {
            std::lock_guard<std::mutex> lock(mutex);
            appendAssistantMessageLocked("Request cancelled.");
            turnFailed = true;
            busy.store(false);
            return;
        }

        if (!httpResponse.error.empty()) {
            parsed.error = "Connection error: " + httpResponse.error;
        } else if (httpResponse.status < 200 || httpResponse.status >= 300) {
            // Reuse the provider parser to pull error.message out of the body.
            ProviderResponse errorBody = provider->parseResponse(httpResponse.body);
            if (isRetryableRateLimit(httpResponse.status, errorBody) &&
                retryAttempt < kMaxRateLimitRetries) {
                const int nextAttempt = retryAttempt + 1;
                const std::chrono::milliseconds delay =
                    retryDelay(httpResponse, errorBody, nextAttempt);
                bool scheduled = false;
                bool cancelled = false;
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (cancelRequested.load()) {
                        appendAssistantMessageLocked("Request cancelled.");
                        turnFailed = true;
                        cancelled = true;
                    } else {
                        pendingRetry = PendingRetry{
                            std::move(request),
                            std::chrono::steady_clock::now() + delay,
                            nextAttempt
                        };
                        retryScheduled.store(true);
                        scheduled = true;
                    }
                }
                if (cancelled) {
                    busy.store(false);
                    return;
                }
                if (scheduled) {
                    Out::warning(
                        "AI provider rate limit reached; retrying in %.2f seconds "
                        "(attempt %d/%d)",
                        static_cast<double>(delay.count()) / 1000.0,
                        nextAttempt, kMaxRateLimitRetries);
                    busy.store(false);
                    return;
                }
            }

            parsed.error = humanizeProviderError(httpResponse.status, errorBody.error);
            Out::error("AI provider request failed (HTTP %ld): %s",
                httpResponse.status,
                errorBody.error.empty() ? httpResponse.body.c_str() : errorBody.error.c_str());
        } else {
            parsed = provider->parseResponse(httpResponse.body);
        }
    } catch (const std::exception& e) {
        parsed.error = e.what();
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!parsed.error.empty()) {
            appendAssistantMessageLocked(parsed.error);
            turnFailed = true;
        } else if (parsed.toolCalls.empty() && parsed.truncated) {
            // A truncated reply often drops an incomplete tool call mid-JSON, so
            // it arrives with no usable tool call. Use provider usage, when
            // available, to distinguish the editor's output cap from a full
            // provider/model context window.
            std::string message = buildTruncationMessage(request, parsed);
            if (!parsed.text.empty()) {
                message = parsed.text + "\n\n" + message;
            }
            appendAssistantMessageLocked(message);
            turnFailed = true;
        } else if (parsed.text.empty() && parsed.toolCalls.empty()) {
            appendAssistantMessageLocked("The provider returned no text or tool calls.");
            turnFailed = true;
        } else {
            ChatMessage assistant;
            assistant.role = ChatRole::Assistant;
            assistant.content = parsed.text;
            assistant.toolCalls = parsed.toolCalls;
            assistant.thinkingBlocks = parsed.thinkingBlocks;
            messages.push_back(assistant);
            ++revision;
            for (const ToolCall& call : parsed.toolCalls) {
                addToolCallProposalLocked(call);
            }
        }
    }

    busy.store(false);
}

void AiService::appendAssistantMessageLocked(const std::string& text) {
    messages.push_back({ChatRole::Assistant, text});
    ++revision;
}

void AiService::addToolCallProposalLocked(const ToolCall& call) {
    ActionProposal proposal;
    proposal.id = nextProposalId++;
    proposal.toolCallId = call.id;
    proposal.toolName = call.name;
    proposal.arguments = call.arguments;
    proposal.readOnly = EditorActionRegistry::isReadOnly(call.name);

    // A wrong-typed argument throws here, outside the worker's try/catch: that would
    // terminate the editor, so route it through the validation-failure path.
    ValidationResult validation;
    try {
        proposal.description = EditorActionRegistry::describe(call.name, call.arguments);
        validation = EditorActionRegistry::validate(call.name, call.arguments);
    } catch (const std::exception& e) {
        if (proposal.description.empty()) {
            proposal.description = call.name;
        }
        validation = {false, "Malformed arguments for " + call.name + ": " + e.what() +
                             ". Re-send the call with each field in its documented type."};
    }

    if (!validation.ok) {
        // Resolve invalid calls immediately and feed the error back so the
        // tool round-trip stays well-formed and the model can recover.
        proposal.executed = true;
        proposal.result = {false, validation.error, Json::object()};

        ChatMessage toolMessage;
        toolMessage.role = ChatRole::Tool;
        toolMessage.toolCallId = call.id;
        toolMessage.toolName = call.name;
        toolMessage.toolDescription = proposal.description;
        toolMessage.toolSuccess = false;
        toolMessage.content = "Error: " + validation.error;
        messages.push_back(toolMessage);
        ++revision;
    }
    proposals.push_back(proposal);
}

} // namespace doriax::editor::ai
