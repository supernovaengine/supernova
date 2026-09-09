// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace doriax::editor {

// A scrollable, wrapped, colored text region whose content can be selected and
// copied with the mouse (drag, double-click word, triple-click line), Ctrl+A,
// Ctrl+C and a right-click menu. Modeled on the Output window's log viewer but
// driven by a simple list of colored paragraphs so it can be reused.
class SelectableTextView {
public:
    struct ColorSpan {
        int begin = 0;           // UTF-8 byte offset, inclusive
        int end = 0;             // UTF-8 byte offset, exclusive
        ImU32 color = 0;
    };

    struct Paragraph {
        std::string text;        // may contain '\n'; pass an empty string for a spacer line
        ImU32 color;
        ImFont* font = nullptr;  // nullptr = default UI font (e.g. set to a mono font for code)
        bool rule = false;       // centered single-line label with a horizontal line on each side
        std::vector<ColorSpan> colorSpans; // optional inline color overrides
    };

    void draw(const char* id, const ImVec2& size,
              const std::vector<Paragraph>& paragraphs, bool scrollToBottom,
              float lineSpacingY = -1.0f);

private:
    ImGuiTextBuffer buf;
    ImVector<int> lineOffsets;        // size = lineCount + 1
    std::vector<ImU32> lineColors;    // one per line
    std::vector<ImU32> byteColors;    // one per UTF-8 byte in buf
    std::vector<ImFont*> lineFonts;   // one per line; nullptr = default font
    std::vector<char> lineHardBreak;  // 1 = real newline, 0 = soft wrap
    std::vector<char> lineRule;       // 1 = rule line (centered text between horizontal lines)

    int selectionStart = -1;
    int selectionEnd = -1;
    bool hasStoredSelection = false;
    bool isSelecting = false;

    bool stickToBottom = true;
    float lastScrollY = 0.0f;

    size_t builtHash = 0;
    float builtWrapWidth = -1.0f;

    void rebuild(const std::vector<Paragraph>& paragraphs, float wrapWidth);
    int clampIndexToCodepointBoundary(int idx) const;
    void selectWordAt(int bufferIndex);
    void selectLineAt(int bufferIndex);
    void selectAll();
    std::string buildCopyText(int startInclusive, int endExclusive) const;
    bool copySelectionToClipboard() const;
    void copyAllToClipboard() const;
    static bool isWordChar(unsigned int cp);
};

} // namespace doriax::editor
