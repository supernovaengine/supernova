// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <iterator>
#include <string>

namespace doriax::editor {

    // Mirrored by DORIAX_CXX_STANDARDS in engine/CMakeLists.txt, first entry is the default
    inline constexpr int cxxStandards[] = {17, 20, 23};

    inline bool isSupportedCxxStandard(int standard) {
        for (int supported : cxxStandards) {
            if (supported == standard) return true;
        }
        return false;
    }

    inline int sanitizeCxxStandard(int standard) {
        return isSupportedCxxStandard(standard) ? standard : cxxStandards[0];
    }

    // "17, 20, or 23", for settings text, AI tool schemas and generated docs
    inline std::string cxxStandardList() {
        const size_t count = std::size(cxxStandards);
        std::string list;
        for (size_t i = 0; i < count; i++) {
            if (i > 0) list += (i + 1 == count) ? ", or " : ", ";
            list += std::to_string(cxxStandards[i]);
        }
        return list;
    }

}
