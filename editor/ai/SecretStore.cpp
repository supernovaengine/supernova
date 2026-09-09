// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "SecretStore.h"

#include "AppSettings.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>

namespace fs = std::filesystem;

namespace doriax::editor::ai {

namespace {

std::mutex g_mutex;
std::map<std::string, std::string> g_keys;
bool g_loaded = false;

fs::path keyFilePath() {
    return AppSettings::getConfigDirectory() / "ai_keys.dat";
}

uint64_t fnv1a(const std::string& value) {
    uint64_t hash = 1469598103934665603ull;
    for (unsigned char c : value) {
        hash ^= c;
        hash *= 1099511628211ull;
    }
    return hash;
}

// Seed bound to this machine/user (config dir path). Keys obfuscated with it are
// not portable to another machine and are not stored in plaintext.
uint64_t deviceSeed() {
    return fnv1a("doriax.ai.keys.v1|" + AppSettings::getConfigDirectory().string());
}

// Symmetric: applying it twice returns the original bytes.
std::string obfuscate(const std::string& data) {
    uint64_t state = deviceSeed();
    std::string out = data;
    for (char& c : out) {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        c = static_cast<char>(static_cast<unsigned char>(c) ^ static_cast<unsigned char>(state & 0xFF));
    }
    return out;
}

std::string toHex(const std::string& data) {
    static const char* digits = "0123456789abcdef";
    std::string out;
    out.reserve(data.size() * 2);
    for (unsigned char c : data) {
        out.push_back(digits[c >> 4]);
        out.push_back(digits[c & 0x0F]);
    }
    return out;
}

bool fromHex(const std::string& hex, std::string& out) {
    if (hex.size() % 2 != 0) return false;
    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    out.clear();
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        int hi = nibble(hex[i]);
        int lo = nibble(hex[i + 1]);
        if (hi < 0 || lo < 0) return false;
        out.push_back(static_cast<char>((hi << 4) | lo));
    }
    return true;
}

void trim(std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) { s.clear(); return; }
    size_t b = s.find_last_not_of(" \t\r\n");
    s = s.substr(a, b - a + 1);
}

void loadLocked() {
    if (g_loaded) return;
    g_loaded = true;

    std::ifstream in(keyFilePath());
    if (!in) return;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string name = line.substr(0, eq);
        std::string hex = line.substr(eq + 1);
        trim(name);
        trim(hex);

        std::string obfuscated;
        if (!fromHex(hex, obfuscated)) continue;
        std::string key = obfuscate(obfuscated);
        if (!key.empty() && !name.empty()) {
            g_keys[name] = key;
        }
    }
}

void saveLocked() {
    fs::path path = keyFilePath();
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);

    std::ofstream out(path, std::ios::trunc);
    if (!out) return;
    fs::permissions(path, fs::perms::owner_read | fs::perms::owner_write,
                    fs::perm_options::replace, ec);

    out << "# Doriax AI API keys - obfuscated and machine-bound. Do not edit or share.\n";
    for (const auto& entry : g_keys) {
        if (entry.second.empty()) continue;
        out << entry.first << " = " << toHex(obfuscate(entry.second)) << "\n";
    }
}

} // namespace

void SecretStore::setApiKey(const std::string& account, const std::string& key) {
    if (account.empty()) return;
    std::lock_guard<std::mutex> lock(g_mutex);
    loadLocked();
    if (key.empty()) {
        g_keys.erase(account);
    } else {
        g_keys[account] = key;
    }
    saveLocked();
}

std::string SecretStore::getApiKey(const std::string& account) {
    std::lock_guard<std::mutex> lock(g_mutex);
    loadLocked();
    auto it = g_keys.find(account);
    return it == g_keys.end() ? std::string() : it->second;
}

bool SecretStore::hasApiKey(const std::string& account) {
    return !getApiKey(account).empty();
}

void SecretStore::clearApiKey(const std::string& account) {
    std::lock_guard<std::mutex> lock(g_mutex);
    loadLocked();
    g_keys.erase(account);
    saveLocked();
}

void SecretStore::renameAccount(const std::string& from, const std::string& to) {
    if (from == to || from.empty() || to.empty()) return;
    std::lock_guard<std::mutex> lock(g_mutex);
    loadLocked();
    auto it = g_keys.find(from);
    if (it == g_keys.end() || g_keys.count(to)) return;
    g_keys[to] = it->second;
    g_keys.erase(it);
    saveLocked();
}

std::vector<ProviderAccount> SecretStore::configuredAccounts(const Settings& settings) {
    std::vector<ProviderAccount> result;
    for (ProviderAccount& account : listAccounts(settings)) {
        const bool ready = accountRequiresApiKey(account.provider)
            ? hasApiKey(account.id) : !account.url.empty();
        if (ready) {
            result.push_back(std::move(account));
        }
    }
    return result;
}

} // namespace doriax::editor::ai
