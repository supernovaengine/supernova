// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace doriax::editor { class Project; }

namespace doriax::editor::ai {

struct ConversationMeta {
    std::string id;
    std::string title;
    int64_t updatedAt = 0; // epoch seconds
};

// Persists chat conversations as JSON files under the project's internal AI
// folder, so history is scoped to the project (conversations reference its
// scenes/resources), mirroring how editors keep chats per workspace.
class ConversationStore {
public:
    explicit ConversationStore(Project* project);

    std::vector<ConversationMeta> list() const;  // newest first
    bool load(const std::string& id, std::vector<ChatMessage>& outMessages) const;
    void save(const std::string& id, const std::string& title, const std::vector<ChatMessage>& messages);
    void remove(const std::string& id);

    static std::string newId();

private:
    Project* project;
    std::filesystem::path conversationsDir() const;
};

} // namespace doriax::editor::ai
