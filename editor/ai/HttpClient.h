// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"

#include <atomic>
#include <filesystem>
#include <string>

namespace doriax::editor::ai {

class HttpClient {
public:
    HttpResponse send(const HttpRequest& request, const std::atomic<bool>* cancel = nullptr) const;
    HttpResponse get(const std::string& url,
                     const std::vector<std::string>& headers = {},
                     const std::atomic<bool>* cancel = nullptr) const;
    HttpResponse downloadToFile(const std::string& url,
                                const std::filesystem::path& destination,
                                const std::vector<std::string>& headers = {},
                                const std::atomic<bool>* cancel = nullptr) const;

    static std::string urlEncode(const std::string& value);
};

} // namespace doriax::editor::ai
