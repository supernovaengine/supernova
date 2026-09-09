// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace doriax::editor {

    class BidiText {
    public:
        // ImGui draws one glyph per codepoint, left to right, with no shaper, so Arabic
        // comes out unjoined and reversed. This reorders runs to visual order and swaps
        // each letter for the presentation form its neighbours call for. Text with
        // nothing right-to-left is returned unchanged.
        static std::string toVisual(const std::string& text);
    };

}
