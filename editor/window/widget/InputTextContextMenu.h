// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <string>

namespace doriax::editor {

class InputTextContextMenu {
public:
    struct Options {
        const char* popupId = nullptr;
        bool readOnly = false;
        bool password = false;
        void* userData = nullptr;
        std::string (*copyRange)(const char* buffer, int start, int end, void* userData) = nullptr;
        void (*afterEdit)(char* buffer, size_t bufferSize,
                          int* cursorPos, int* selectionStart, int* selectionEnd,
                          void* userData) = nullptr;
    };

    static bool drawForLastItem(char* buffer, size_t bufferSize);
    static bool drawForLastItem(char* buffer, size_t bufferSize,
                                const Options& options);
};

} // namespace doriax::editor
