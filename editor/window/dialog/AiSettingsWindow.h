// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "ai/AiTypes.h"
#include "imgui.h"

#include <array>
#include <map>
#include <string>
#include <vector>

namespace doriax::editor {

namespace ai { class AiService; }

// Modal dialog that manages API keys (add one, switch provider, add another -
// like VS Code) plus the list of named OpenAI-compatible endpoints and the
// request limits. Model and approval controls live in the chat composer.
class AiSettingsWindow {
private:
    // std::map keeps buffer addresses stable for ImGui as endpoints come and go.
    struct KeyEntry {
        std::array<char, 512> buffer{};
        bool configured = false;
    };
    struct EndpointEntry {
        std::array<char, 128> label{};
        std::array<char, 512> url{};
    };

    bool m_isOpen = false;
    ai::AiService* m_service = nullptr;

    ai::Settings m_settings;
    std::map<std::string, KeyEntry> m_keys;          // account id -> typed key
    std::map<std::string, EndpointEntry> m_endpoints; // endpoint id -> typed fields
    // Endpoints removed here. Wiped only on OK, so Cancel restores their keys.
    std::vector<std::string> m_removedAccounts;
    // Endpoint being renamed in place, and its one-frame focus request.
    std::string m_editingEndpointId;
    bool m_focusEndpointLabel = false;
    int m_requestTimeoutSeconds = 90;
    int m_maxOutputTokens = 8192;
    int m_maxToolRounds = 24;

    void drawSettings();
    void drawAccountKeyRow(const ai::ProviderAccount& account, bool dimLabel = false);
    void drawEndpointRows();
    void drawAddEndpointButton();
    void addEndpoint(const std::string& label, const std::string& url);
    void removeEndpoint(const std::string& endpointId);
    void syncBuffers();
    void refreshKeyState();
    void apply();

public:
    AiSettingsWindow() = default;
    ~AiSettingsWindow() = default;

    void open(ai::AiService* service);
    void show();
    bool isOpen() const { return m_isOpen; }
};

} // namespace doriax::editor
