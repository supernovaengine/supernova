// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"
#include "HttpClient.h"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace doriax::editor::ai {

struct ModelInfo {
    std::string id;
    std::string label;
};

enum class CatalogStatus { Idle, Loading, Loaded, Error };

// Fetches the models each account exposes (OpenAI /v1/models, Anthropic
// /v1/models, Gemini ListModels, DeepSeek /models, and the /models route derived
// from a custom endpoint's URL) on a background thread, cached per account.
class ModelCatalog {
public:
    ModelCatalog();
    ~ModelCatalog();

    // Cached models for an account, empty until a fetch lands.
    std::vector<ModelInfo> models(const std::string& account) const;
    CatalogStatus status(const std::string& account) const;

    // True when nothing is cached yet, or the endpoint was re-pointed at a new URL.
    bool needsFetch(const ProviderAccount& account) const;

    // Queues a background fetch (forces a re-fetch even if already loaded).
    void refresh(const ProviderAccount& account, const std::string& apiKey);

    // Builds a friendly label from a raw model id when a provider exposes no
    // display name (e.g. "deepseek-v4-flash" -> "Deepseek V4 Flash").
    static std::string humanizeModelId(const std::string& id);

private:
    struct Request {
        ProviderId provider;
        std::string account;
        std::string url;
        std::string apiKey;
    };
    struct Entry {
        CatalogStatus status = CatalogStatus::Idle;
        std::vector<ModelInfo> models;
        std::string url; // what it was fetched from, so a re-pointed endpoint refetches
    };

    void workerLoop();
    bool fetchModels(const Request& request, std::vector<ModelInfo>& out);

    mutable std::mutex mutex;
    std::condition_variable condition;
    std::map<std::string, Entry> cache;
    std::deque<Request> queue;
    std::thread worker;
    bool stop = false;
    HttpClient httpClient;
};

} // namespace doriax::editor::ai
