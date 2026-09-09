// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"

#include <string>

namespace doriax::editor::ai {

class AiEngineApiContext {
public:
    static Json search(const std::string& query,
                       const std::string& parentFilter,
                       int maxResults);
};

} // namespace doriax::editor::ai
