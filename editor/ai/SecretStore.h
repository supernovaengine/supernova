// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"

#include <string>
#include <vector>

namespace doriax::editor::ai {

// Per-account API key store. Keys are persisted to a 0600 file in the user
// config directory, lightly obfuscated with a machine-derived keystream so they
// are not written in plaintext. This protects against casual reads and is not as
// strong as an OS keychain. Accounts are the ids from ai::accountKey(), so every
// provider and custom endpoint holds its own key.
class SecretStore {
public:
    static void setApiKey(const std::string& account, const std::string& key);
    static std::string getApiKey(const std::string& account);
    static bool hasApiKey(const std::string& account);
    static void clearApiKey(const std::string& account);

    // Moves a key onto a new account id, so the legacy single custom-endpoint
    // slot survives the upgrade to named endpoints. Never overwrites.
    static void renameAccount(const std::string& from, const std::string& to);

    // Accounts ready to use, in listAccounts() order: a built-in needs a stored
    // key, a custom endpoint only needs a URL.
    static std::vector<ProviderAccount> configuredAccounts(const Settings& settings);
};

} // namespace doriax::editor::ai
