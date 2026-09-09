// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ModelCatalog.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <set>

namespace doriax::editor::ai {

namespace {

// Upper bound for the picker; gateways list hundreds.
constexpr size_t kMaxModels = 100;
constexpr int64_t kRecentOpenAiModelWindowSeconds = 60LL * 60LL * 24LL * 365LL * 2LL;

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool startsWithAny(const std::string& value, std::initializer_list<const char*> prefixes) {
    for (const char* prefix : prefixes) {
        if (value.rfind(prefix, 0) == 0) return true;
    }
    return false;
}

bool endsWith(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool containsAny(const std::string& value, std::initializer_list<const char*> needles) {
    for (const char* needle : needles) {
        if (value.find(needle) != std::string::npos) return true;
    }
    return false;
}

bool isDigitAt(const std::string& value, size_t index) {
    return index < value.size() && std::isdigit(static_cast<unsigned char>(value[index]));
}

bool containsIsoDateToken(const std::string& id) {
    for (size_t i = 0; i + 10 <= id.size(); ++i) {
        if ((i == 0 || id[i - 1] == '-') &&
            isDigitAt(id, i) && isDigitAt(id, i + 1) &&
            isDigitAt(id, i + 2) && isDigitAt(id, i + 3) &&
            id[i + 4] == '-' &&
            isDigitAt(id, i + 5) && isDigitAt(id, i + 6) &&
            id[i + 7] == '-' &&
            isDigitAt(id, i + 8) && isDigitAt(id, i + 9)) {
            return true;
        }
    }
    return false;
}

bool containsCompactDateToken(const std::string& id) {
    for (size_t i = 0; i + 5 <= id.size(); ++i) {
        if (id[i] != '-') continue;
        if (!isDigitAt(id, i + 1) || !isDigitAt(id, i + 2) ||
            !isDigitAt(id, i + 3) || !isDigitAt(id, i + 4)) {
            continue;
        }
        const bool tokenEnds = i + 5 == id.size() || id[i + 5] == '-';
        if (tokenEnds) return true;
    }
    return false;
}

bool looksLikeOpenAiChatFamily(const std::string& id) {
    return startsWithAny(id, {"gpt-", "chatgpt-", "o1", "o3", "o4"});
}

bool looksLikeMainOpenAiChatModel(const std::string& id, int64_t created, int64_t newestCreated) {
    const std::string token = lower(id);
    if (!looksLikeOpenAiChatFamily(token)) return false;

    // Keep chat aliases (including *-preview). Drop specialized modalities and
    // non-chat endpoints that share the gpt-/o* namespace.
    if (containsAny(token, {
            "audio", "batch", "computer-use", "dall-e", "edit", "embedding",
            "image", "instruct", "moderation", "realtime",
            "search", "transcribe", "tts", "vision", "whisper"
        })) {
        return false;
    }

    // Provider model lists expose both aliases and dated snapshots. The aliases
    // (stable and preview) are the entries users normally want in a picker.
    if (containsIsoDateToken(token) || containsCompactDateToken(token)) {
        return false;
    }

    // Drop old stable aliases without pinning specific model ids. If the provider
    // omits creation times, keep the alias and let the user choose.
    if (created > 0 && newestCreated > 0 &&
        newestCreated - created > kRecentOpenAiModelWindowSeconds) {
        return false;
    }

    return true;
}

bool isOpenCodeZen(const std::string& url) {
    return lower(url).find("opencode.ai") != std::string::npos;
}

// Zen routes each vendor family to /chat/completions, /responses, /messages or a
// Gemini path, and /models lists all of them without saying which.
bool servedByChatCompletions(const std::string& id) {
    const std::string token = lower(id);
    return !startsWithAny(token, {"claude-", "gpt-", "grok-", "gemini-", "qwen", "muse-spark"});
}

bool looksLikeMainGeminiModel(const std::string& id) {
    const std::string token = lower(id);
    if (!startsWithAny(token, {"gemini-"})) return false;

    // Specialized / non-chat modalities stay out of the picker.
    if (containsAny(token, {
            "audio", "computer-use", "deep-research", "embedding", "image",
            "imagen", "live", "lyria", "nano-banana", "robotics", "tts", "veo"
        })) {
        return false;
    }

    // Main chat families: pro / flash / flash-lite, including -preview aliases.
    if (!containsAny(token, {"-pro", "-flash"})) return false;

    // Dated snapshots (e.g. gemini-2.5-flash-preview-09-2025) are noisy; keep
    // undated stable and preview ids.
    if (containsIsoDateToken(token) || containsCompactDateToken(token)) {
        return false;
    }

    return true;
}

} // namespace

ModelCatalog::ModelCatalog() {
    worker = std::thread(&ModelCatalog::workerLoop, this);
}

ModelCatalog::~ModelCatalog() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;
    }
    condition.notify_all();
    if (worker.joinable()) {
        worker.join();
    }
}

std::vector<ModelInfo> ModelCatalog::models(const std::string& account) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = cache.find(account);
    if (it != cache.end() && it->second.status == CatalogStatus::Loaded && !it->second.models.empty()) {
        return it->second.models;
    }
    // No fabricated fallback: the list is only what the provider actually returns.
    return {};
}

CatalogStatus ModelCatalog::status(const std::string& account) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = cache.find(account);
    return it == cache.end() ? CatalogStatus::Idle : it->second.status;
}

bool ModelCatalog::needsFetch(const ProviderAccount& account) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = cache.find(account.id);
    if (it == cache.end() || it->second.status == CatalogStatus::Idle) {
        return true;
    }
    if (it->second.status == CatalogStatus::Loading) {
        return false;
    }
    // A failed fetch is not retried on its own; a new URL is.
    return it->second.url != account.url;
}

void ModelCatalog::refresh(const ProviderAccount& account, const std::string& apiKey) {
    if (account.id.empty() ||
        (apiKey.empty() && accountRequiresApiKey(account.provider))) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutex);
        Entry& entry = cache[account.id];
        if (entry.status == CatalogStatus::Loading) {
            return; // already in flight
        }
        entry.status = CatalogStatus::Loading;
        queue.push_back({account.provider, account.id, account.url, apiKey});
    }
    condition.notify_one();
}

void ModelCatalog::workerLoop() {
    for (;;) {
        Request request;
        {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [this] { return stop || !queue.empty(); });
            if (stop) {
                return;
            }
            request = queue.front();
            queue.pop_front();
        }

        std::vector<ModelInfo> fetched;
        bool ok = fetchModels(request, fetched);

        std::lock_guard<std::mutex> lock(mutex);
        Entry& entry = cache[request.account];
        entry.url = request.url; // on failure too, so a broken URL is not retried every frame
        if (ok && !fetched.empty()) {
            entry.models = std::move(fetched);
            entry.status = CatalogStatus::Loaded;
        } else {
            entry.status = CatalogStatus::Error;
        }
    }
}

bool ModelCatalog::fetchModels(const Request& request, std::vector<ModelInfo>& out) {
    std::string url;
    std::vector<std::string> headers = {"Accept: application/json"};

    switch (request.provider) {
        case ProviderId::OpenAI:
            url = "https://api.openai.com/v1/models";
            headers.push_back("Authorization: Bearer " + request.apiKey);
            break;
        case ProviderId::Anthropic:
            url = "https://api.anthropic.com/v1/models?limit=100";
            headers.push_back("x-api-key: " + request.apiKey);
            headers.push_back("anthropic-version: 2023-06-01");
            break;
        case ProviderId::Gemini:
            url = "https://generativelanguage.googleapis.com/v1beta/models?pageSize=200&key=" +
                  HttpClient::urlEncode(request.apiKey);
            break;
        case ProviderId::DeepSeek:
            url = "https://api.deepseek.com/models";
            headers.push_back("Authorization: Bearer " + request.apiKey);
            break;
        case ProviderId::OpenAICompatible: {
            if (request.url.empty()) return false;
            size_t pos = request.url.find("/chat/completions");
            url = (pos != std::string::npos) ? request.url.substr(0, pos) + "/models"
                                             : request.url;
            if (!request.apiKey.empty()) {
                headers.push_back("Authorization: Bearer " + request.apiKey);
            }
            break;
        }
    }

    HttpResponse response = httpClient.get(url, headers);
    if (!response.error.empty() || response.status < 200 || response.status >= 300) {
        return false;
    }

    Json root = Json::parse(response.body, nullptr, false);
    if (!root.is_object()) {
        return false;
    }

    if (request.provider == ProviderId::Gemini) {
        if (!root.contains("models") || !root["models"].is_array()) return false;
        for (const Json& model : root["models"]) {
            const Json& methods = model.value("supportedGenerationMethods", Json::array());
            bool generates = false;
            if (methods.is_array()) {
                for (const Json& m : methods) {
                    if (m.is_string() && m.get<std::string>() == "generateContent") { generates = true; break; }
                }
            }
            if (!generates) continue;
            std::string name = model.value("name", "");
            const std::string prefix = "models/";
            if (name.rfind(prefix, 0) == 0) name = name.substr(prefix.size());
            if (name.empty()) continue;
            std::string label = model.value("displayName", std::string());
            if (label.empty()) label = humanizeModelId(name);
            if (!looksLikeMainGeminiModel(name)) continue;
            out.push_back({name, label});
            if (out.size() >= kMaxModels) break;
        }
        return !out.empty();
    }

    // OpenAI, Anthropic and OpenAI-compatible all return { "data": [ ... ] }.
    if (!root.contains("data") || !root["data"].is_array()) {
        return false;
    }

    if (request.provider == ProviderId::OpenAI) {
        struct OpenAiModelCandidate {
            std::string id;
            std::string label;
            int64_t created = 0;
        };

        std::vector<OpenAiModelCandidate> candidates;
        int64_t newestCreated = 0;
        for (const Json& model : root["data"]) {
            std::string id = model.value("id", "");
            if (id.empty()) continue;
            int64_t created = 0;
            if (model.contains("created") && model["created"].is_number_integer()) {
                created = model["created"].get<int64_t>();
                newestCreated = std::max(newestCreated, created);
            }
            std::string label = model.value("display_name", model.value("name", std::string()));
            if (label.empty()) label = humanizeModelId(id);
            candidates.push_back({id, label, created});
        }

        std::sort(candidates.begin(), candidates.end(), [](const OpenAiModelCandidate& a,
                                                           const OpenAiModelCandidate& b) {
            if (a.created != b.created) return a.created > b.created;
            return a.id < b.id;
        });

        for (const OpenAiModelCandidate& model : candidates) {
            if (!looksLikeMainOpenAiChatModel(model.id, model.created, newestCreated)) {
                continue;
            }
            out.push_back({model.id, model.label});
            if (out.size() >= kMaxModels) break;
        }
        return !out.empty();
    }

    if (isOpenCodeZen(request.url)) {
        // Free models lead the list. Zen suffixes them "-free" and exposes no
        // cost field, so the one that skips the convention is named here.
        static const std::set<std::string> freeWithoutSuffix = {"big-pickle"};
        std::vector<ModelInfo> freeModels;
        std::vector<ModelInfo> paidModels;
        for (const Json& model : root["data"]) {
            std::string id = model.value("id", "");
            if (id.empty() || !servedByChatCompletions(id)) continue;
            std::string label = model.value("display_name", model.value("name", std::string()));
            if (label.empty()) label = humanizeModelId(id);
            const std::string token = lower(id);
            const bool isFree = endsWith(token, "-free") || freeWithoutSuffix.count(token) > 0;
            (isFree ? freeModels : paidModels).push_back({id, label});
        }
        for (std::vector<ModelInfo>* group : {&freeModels, &paidModels}) {
            for (ModelInfo& model : *group) {
                if (out.size() >= kMaxModels) break;
                out.push_back(std::move(model));
            }
        }
        return !out.empty();
    }

    for (const Json& model : root["data"]) {
        std::string id = model.value("id", "");
        if (id.empty()) continue;
        std::string label = model.value("display_name", model.value("name", std::string()));
        if (label.empty()) label = humanizeModelId(id);
        out.push_back({id, label});
        if (out.size() >= kMaxModels) break;
    }
    return !out.empty();
}

std::string ModelCatalog::humanizeModelId(const std::string& id) {
    // Title-case each hyphen/slash/underscore separated token, leaving version
    // tokens ("4o", "3.5", "70b") untouched and upper-casing known acronyms.
    static const std::set<std::string> acronyms = {"gpt", "ai", "llm", "tts", "hd"};
    std::string out;
    std::string token;
    auto flush = [&]() {
        if (token.empty()) return;
        if (!out.empty()) out.push_back(' ');
        if (acronyms.count(token)) {
            for (char c : token) out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
        } else if (std::isalpha(static_cast<unsigned char>(token[0]))) {
            out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(token[0]))));
            out.append(token, 1, std::string::npos);
        } else {
            out += token; // version-like token, keep verbatim
        }
        token.clear();
    };
    for (char c : id) {
        if (c == '-' || c == '_' || c == '/' || c == ' ' || c == ':') {
            flush();
        } else {
            token.push_back(c); // '.' stays inside the token so "3.5" survives
        }
    }
    flush();
    return out.empty() ? id : out;
}

} // namespace doriax::editor::ai
