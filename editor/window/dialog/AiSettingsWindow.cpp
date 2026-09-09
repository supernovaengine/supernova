// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "AiSettingsWindow.h"

#include "AppSettings.h"
#include "ai/AiService.h"
#include "ai/AiTypes.h"
#include "ai/SecretStore.h"
#include "external/IconsFontAwesome6.h"
#include "window/Widgets.h"

#include <algorithm>
#include <cstdio>

namespace doriax::editor {

namespace {

const ImVec4 kKeySetColor(0.55f, 0.85f, 0.55f, 1.0f);

void setBuffer(char* buffer, size_t size, const std::string& value) {
    std::snprintf(buffer, size, "%s", value.c_str());
}

// One column geometry for every section, so their dividers line up.
bool beginSettingsTable(const char* id) {
    if (!ImGui::BeginTable(id, 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
        return false;
    }
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 160);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
    return true;
}

// Borderless dim icon after a label, matching the reset-to-default buttons.
bool inlineIconButton(const char* icon, const char* id, const char* tooltip) {
    ImGui::SameLine(0.0f, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, ImGui::GetStyle().FramePadding.y));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
    bool clicked = ImGui::Button((std::string(icon) + "##" + id).c_str());
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
    return clicked;
}

} // namespace

void AiSettingsWindow::open(ai::AiService* service) {
    m_isOpen = true;
    m_service = service;

    m_settings = AppSettings::getAiSettings();
    m_requestTimeoutSeconds = m_settings.requestTimeoutSeconds;
    m_maxOutputTokens = m_settings.maxOutputTokens;
    m_maxToolRounds = m_settings.maxToolRounds;
    m_keys.clear();
    m_endpoints.clear();
    m_removedAccounts.clear();
    m_editingEndpointId.clear();
    m_focusEndpointLabel = false;
    syncBuffers();
}

// Adds buffers for new rows and drops gone ones, keeping typed text intact.
void AiSettingsWindow::syncBuffers() {
    std::vector<ai::ProviderAccount> accounts = ai::listAccounts(m_settings);

    std::map<std::string, KeyEntry> keys;
    for (const ai::ProviderAccount& account : accounts) {
        auto existing = m_keys.find(account.id);
        keys[account.id] = existing != m_keys.end() ? existing->second : KeyEntry{};
    }
    m_keys = std::move(keys);

    std::map<std::string, EndpointEntry> endpoints;
    for (const ai::CustomEndpoint& endpoint : m_settings.customEndpoints) {
        auto existing = m_endpoints.find(endpoint.id);
        if (existing != m_endpoints.end()) {
            endpoints[endpoint.id] = existing->second;
            continue;
        }
        EndpointEntry entry;
        setBuffer(entry.label.data(), entry.label.size(), endpoint.label);
        setBuffer(entry.url.data(), entry.url.size(), endpoint.url);
        endpoints[endpoint.id] = entry;
    }
    m_endpoints = std::move(endpoints);

    refreshKeyState();
}

void AiSettingsWindow::refreshKeyState() {
    for (auto& entry : m_keys) {
        entry.second.configured = ai::SecretStore::hasApiKey(entry.first);
    }
}

void AiSettingsWindow::addEndpoint(const std::string& label, const std::string& url) {
    ai::CustomEndpoint endpoint;
    endpoint.id = ai::makeEndpointId(m_settings, label);
    endpoint.label = label;
    endpoint.url = url;
    m_settings.customEndpoints.push_back(endpoint);
    syncBuffers();
}

void AiSettingsWindow::removeEndpoint(const std::string& endpointId) {
    m_removedAccounts.push_back(ai::accountKey(ai::ProviderId::OpenAICompatible, endpointId));
    auto& endpoints = m_settings.customEndpoints;
    endpoints.erase(std::remove_if(endpoints.begin(), endpoints.end(),
                                   [&](const ai::CustomEndpoint& endpoint) {
                                       return endpoint.id == endpointId;
                                   }),
                    endpoints.end());
    // A deleted endpoint left selected would send to an empty URL.
    if (m_settings.endpointId == endpointId) {
        m_settings.endpointId.clear();
    }
    if (m_editingEndpointId == endpointId) {
        m_editingEndpointId.clear();
    }
    syncBuffers();
}

void AiSettingsWindow::apply() {
    // The id never changes, so a rename keeps the endpoint's stored key.
    for (ai::CustomEndpoint& endpoint : m_settings.customEndpoints) {
        auto it = m_endpoints.find(endpoint.id);
        if (it == m_endpoints.end()) continue;
        endpoint.label = it->second.label.data();
        endpoint.url = it->second.url.data();
    }

    ai::Settings settings = m_settings;
    settings.requestTimeoutSeconds = std::clamp(m_requestTimeoutSeconds, 1, 3600);
    settings.maxOutputTokens = std::clamp(m_maxOutputTokens, 256, 16000);
    settings.maxToolRounds = std::clamp(m_maxToolRounds, 1, 100);
    AppSettings::setAiSettings(settings);
    m_settings = settings;
    if (m_service) {
        m_service->setSettings(settings);
    }

    // Deleted endpoints lose their key, unless the same id was re-added before OK.
    for (const std::string& account : m_removedAccounts) {
        bool restored = false;
        for (const ai::CustomEndpoint& endpoint : settings.customEndpoints) {
            if (ai::accountKey(ai::ProviderId::OpenAICompatible, endpoint.id) == account) {
                restored = true;
                break;
            }
        }
        if (!restored) {
            ai::SecretStore::clearApiKey(account);
        }
    }
    m_removedAccounts.clear();

    // Persist any keys the user typed (one per account). Keys are stored
    // obfuscated in the user config dir, never alongside the project.
    for (auto& entry : m_keys) {
        if (entry.second.buffer[0] != '\0') {
            ai::SecretStore::setApiKey(entry.first, entry.second.buffer.data());
            entry.second.buffer.fill('\0');
        }
    }
    refreshKeyState();
}

void AiSettingsWindow::show() {
    if (!m_isOpen) return;

    ImGui::OpenPopup("AI Settings##AiSettingsModal");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(560, 0),
        ImVec2(560, ImGui::GetMainViewport()->WorkSize.y * 0.9f)
    );

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::BeginPopupModal("AI Settings##AiSettingsModal", &m_isOpen, flags)) {
        if (!m_isOpen) {
            ImGui::CloseCurrentPopup();
        } else {
            drawSettings();
        }
        ImGui::EndPopup();
    }
}

void AiSettingsWindow::drawAccountKeyRow(const ai::ProviderAccount& account, bool dimLabel) {
    KeyEntry& entry = m_keys[account.id];
    float clearWidth = ImGui::GetFrameHeight();
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    if (dimLabel) {
        ImGui::TextDisabled("%s", account.label.c_str());
    } else {
        ImGui::Text("%s", account.label.c_str());
    }
    if (entry.configured) {
        ImGui::SameLine();
        ImGui::TextColored(kKeySetColor, ICON_FA_CIRCLE_CHECK);
        ImGui::SetItemTooltip("Key configured");
    }

    ImGui::TableNextColumn();
    ImGui::PushID(account.id.c_str());
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - clearWidth - spacing);
    ImGui::InputTextWithHint("##Key", entry.configured ? "key set - type to replace" : "paste API key",
                             entry.buffer.data(), entry.buffer.size(),
                             ImGuiInputTextFlags_Password);
    if (ImGui::BeginPopupContextItem("##KeyContext")) {
        if (ImGui::MenuItem(ICON_FA_CLIPBOARD " Paste")) {
            const char* clipboard = ImGui::GetClipboardText();
            if (clipboard) {
                setBuffer(entry.buffer.data(), entry.buffer.size(), clipboard);
            }
        }
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(!entry.configured && entry.buffer[0] == '\0');
    if (Widgets::iconButton("##ClearKey", ICON_FA_TRASH, ImVec2(clearWidth, ImGui::GetFrameHeight()))) {
        ai::SecretStore::clearApiKey(account.id);
        entry.buffer.fill('\0');
        refreshKeyState();
    }
    ImGui::EndDisabled();
    ImGui::SetItemTooltip("Clear this key");
    ImGui::PopID();
}

void AiSettingsWindow::drawEndpointRows() {
    std::string pendingRemoval;
    for (const ai::CustomEndpoint& endpoint : m_settings.customEndpoints) {
        auto it = m_endpoints.find(endpoint.id);
        if (it == m_endpoints.end()) continue;
        EndpointEntry& fields = it->second;

        float clearWidth = ImGui::GetFrameHeight();
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::PushID(endpoint.id.c_str());
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        // A plain label until the pencil turns it into a field.
        if (m_editingEndpointId == endpoint.id) {
            ImGui::SetNextItemWidth(-1);
            if (m_focusEndpointLabel) {
                ImGui::SetKeyboardFocusHere();
                m_focusEndpointLabel = false;
            }
            if (ImGui::InputText("##Label", fields.label.data(), fields.label.size(),
                                 ImGuiInputTextFlags_EnterReturnsTrue) ||
                ImGui::IsItemDeactivated()) {
                m_editingEndpointId.clear();
            }
        } else {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", fields.label[0] != '\0' ? fields.label.data() : endpoint.id.c_str());
            if (inlineIconButton(ICON_FA_PENCIL, "EditLabel", "Rename this endpoint")) {
                m_editingEndpointId = endpoint.id;
                m_focusEndpointLabel = true;
            }
        }

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - clearWidth - spacing);
        ImGui::InputTextWithHint("##Url", "https://host/v1/chat/completions",
                                 fields.url.data(), fields.url.size());
        ImGui::SameLine();
        if (Widgets::iconButton("##RemoveEndpoint", ICON_FA_XMARK,
                                ImVec2(clearWidth, ImGui::GetFrameHeight()))) {
            pendingRemoval = endpoint.id;
        }
        ImGui::SetItemTooltip("Remove this endpoint and its key");
        ImGui::PopID();

        ai::ProviderAccount account;
        account.provider = ai::ProviderId::OpenAICompatible;
        account.endpointId = endpoint.id;
        account.label = "API key";
        account.id = ai::accountKey(ai::ProviderId::OpenAICompatible, endpoint.id);
        account.url = endpoint.url;
        drawAccountKeyRow(account, true);
    }

    // Deferred so the endpoint list is not mutated mid-iteration.
    if (!pendingRemoval.empty()) {
        removeEndpoint(pendingRemoval);
    }
}

// Outside the table, centered like the section header above it.
void AiSettingsWindow::drawAddEndpointButton() {
    const float width = 160.0f;
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
    if (ImGui::Button(ICON_FA_PLUS "  Add endpoint", ImVec2(width, 0))) {
        ImGui::OpenPopup("##AddEndpoint");
    }
    if (ImGui::BeginPopup("##AddEndpoint")) {
        // The presets below only prefill a URL that could be typed here instead.
        if (ImGui::MenuItem("OpenAI compatible")) {
            addEndpoint("OpenAI compatible", "");
        }
        ImGui::Separator();
        for (const ai::EndpointPreset& preset : ai::endpointPresets()) {
            if (ImGui::MenuItem(preset.label.c_str())) {
                addEndpoint(preset.label, preset.url);
            }
        }
        ImGui::EndPopup();
    }
}

void AiSettingsWindow::drawSettings() {
    ImGui::TextDisabled("Add a key for any provider; keys are stored obfuscated in your");
    ImGui::TextDisabled("user config folder. The model picker lists configured providers.");
    ImGui::Spacing();

    // Default-constructed settings carry the field defaults (kept in sync with AiTypes.h).
    const ai::Settings defaults;

    // Borderless rotate-left "reset to default" button placed after a label, mirroring the
    // pattern in Properties.cpp. Returns true when clicked.
    auto resetToDefaultButton = [](const char* id, const std::string& tooltip) -> bool {
        ImGui::SameLine(0.0f, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, ImGui::GetStyle().FramePadding.y));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        bool clicked = ImGui::Button((std::string(ICON_FA_ROTATE_LEFT) + "##" + id).c_str());
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tooltip.c_str());
        }
        return clicked;
    };

    if (beginSettingsTable("ai_provider_keys")) {
        for (const ai::ProviderAccount& account : ai::listAccounts(m_settings)) {
            if (account.provider == ai::ProviderId::OpenAICompatible) continue;
            drawAccountKeyRow(account);
        }
        ImGui::EndTable();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.5f, 0.5f));
    ImGui::SeparatorText("Custom endpoints");
    ImGui::PopStyleVar();
    ImGui::SetItemTooltip("OpenAI-compatible Chat Completions URLs. Each keeps its own key and "
                          "model list, so several can be configured at the same time.");

    // Skipped when empty: a zero-row table still costs its padding.
    if (!m_settings.customEndpoints.empty() && beginSettingsTable("ai_endpoints")) {
        drawEndpointRows();
        ImGui::EndTable();
    }

    drawAddEndpointButton();

    ImGui::Separator();

    if (beginSettingsTable("ai_limits")) {
        // Per-request timeout
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Request Timeout (s)");
        ImGui::SetItemTooltip(
            "Maximum time to wait for each AI model response. Raise this for slow local models.");
        if (m_requestTimeoutSeconds != defaults.requestTimeoutSeconds &&
            resetToDefaultButton("reset_requesttimeout",
                                 "Reset to default (" + std::to_string(defaults.requestTimeoutSeconds) + " seconds)")) {
            m_requestTimeoutSeconds = defaults.requestTimeoutSeconds;
        }
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputInt("##RequestTimeout", &m_requestTimeoutSeconds, 30, 60);
        m_requestTimeoutSeconds = std::clamp(m_requestTimeoutSeconds, 1, 3600);

        // Max output tokens
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Max Output Tokens");
        if (m_maxOutputTokens != defaults.maxOutputTokens &&
            resetToDefaultButton("reset_maxoutput", "Reset to default (" + std::to_string(defaults.maxOutputTokens) + ")")) {
            m_maxOutputTokens = defaults.maxOutputTokens;
        }
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputInt("##MaxOutput", &m_maxOutputTokens, 256, 1024);
        m_maxOutputTokens = std::clamp(m_maxOutputTokens, 256, 16000);

        // Max tool steps (agent loop continuations after tool results)
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Max Tool Steps");
        ImGui::SetItemTooltip(
            "How many model turns that request tools are allowed per user message. "
            "Raise this if you see \"Reached the tool-step limit\" on long tasks.");
        if (m_maxToolRounds != defaults.maxToolRounds &&
            resetToDefaultButton("reset_maxtoolrounds",
                                 "Reset to default (" + std::to_string(defaults.maxToolRounds) + ")")) {
            m_maxToolRounds = defaults.maxToolRounds;
        }
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputInt("##MaxToolRounds", &m_maxToolRounds, 1, 4);
        m_maxToolRounds = std::clamp(m_maxToolRounds, 1, 100);

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float windowWidth = ImGui::GetWindowSize().x;
    float buttonsWidth = 250;
    ImGui::SetCursorPosX((windowWidth - buttonsWidth) * 0.5f);

    if (ImGui::Button("OK", ImVec2(120, 0))) {
        apply();
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        m_isOpen = false;
        ImGui::CloseCurrentPopup();
    }
}

} // namespace doriax::editor
