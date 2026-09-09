// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "SelectableTextView.h"

#include "external/IconsFontAwesome6.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cmath>
#include <functional>

namespace doriax::editor {

void SelectableTextView::rebuild(const std::vector<Paragraph>& paragraphs, float wrapWidth) {
    buf.clear();
    lineOffsets.clear();
    lineOffsets.push_back(0);
    lineColors.clear();
    byteColors.clear();
    lineFonts.clear();
    lineHardBreak.clear();
    lineRule.clear();

    if (wrapWidth <= 0.0f) {
        wrapWidth = 1.0f;
    }

    ImFont* defaultFont = ImGui::GetFont();
    const float fontSize = ImGui::GetFontSize();

    auto pushLine = [&](const std::string& line, const std::vector<ImU32>& lineByteColors,
                        ImU32 color, ImFont* lineFont,
                        bool hardBreak, bool appendNewline, bool rule = false) {
        buf.append(line.data(), line.data() + line.size());
        byteColors.insert(byteColors.end(), lineByteColors.begin(), lineByteColors.end());
        if (appendNewline) {
            buf.append("\n");
            byteColors.push_back(color);
        }
        lineOffsets.push_back(buf.size());
        lineColors.push_back(color);
        lineFonts.push_back(lineFont);
        lineHardBreak.push_back(hardBreak ? 1 : 0);
        lineRule.push_back(rule ? 1 : 0);
    };

    for (size_t pi = 0; pi < paragraphs.size(); ++pi) {
        const Paragraph& para = paragraphs[pi];
        const bool lastPara = (pi + 1 == paragraphs.size());
        ImFont* font = para.font ? para.font : defaultFont;

        std::vector<ImU32> sourceColors(para.text.size(), para.color);
        for (const ColorSpan& span : para.colorSpans) {
            const int begin = std::clamp(span.begin, 0, static_cast<int>(para.text.size()));
            const int end = std::clamp(span.end, begin, static_cast<int>(para.text.size()));
            std::fill(sourceColors.begin() + begin, sourceColors.begin() + end, span.color);
        }

        if (para.rule) {
            // Rule labels are short; keep them on one centered line instead of wrapping.
            pushLine(para.text, sourceColors, para.color, para.font, true, !lastPara, true);
            continue;
        }

        std::string currentLine;
        std::vector<ImU32> currentColors;
        float currentWidth = 0.0f;
        int lastBreakPos = 0;       // byte offset in currentLine just past the last space
        float widthAtBreak = 0.0f;  // width of currentLine up to lastBreakPos
        const char* s = para.text.data();
        const char* e = s + para.text.size();

        while (s < e) {
            unsigned int cp = 0;
            int len = ImTextCharFromUtf8(&cp, s, e);
            if (len <= 0) {
                cp = static_cast<unsigned char>(*s);
                len = 1;
            }
            if (cp == '\r') {
                s += len;
                continue;
            }
            if (cp == '\n') {
                pushLine(currentLine, currentColors, para.color, para.font, true, true);
                currentLine.clear();
                currentColors.clear();
                currentWidth = 0.0f;
                lastBreakPos = 0;
                widthAtBreak = 0.0f;
                s += len;
                continue;
            }

            float cw = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, s, s + len).x;
            if (currentWidth + cw > wrapWidth && !currentLine.empty()) {
                if (lastBreakPos > 0 && lastBreakPos < (int)currentLine.size()) {
                    // Break after the last space; carry the trailing partial word
                    // to the next line. The space stays so copies rejoin cleanly.
                    std::string tail = currentLine.substr(lastBreakPos);
                    std::vector<ImU32> tailColors(currentColors.begin() + lastBreakPos,
                                                  currentColors.end());
                    std::vector<ImU32> headColors(currentColors.begin(),
                                                 currentColors.begin() + lastBreakPos);
                    pushLine(currentLine.substr(0, lastBreakPos), headColors,
                             para.color, para.font, false, true);
                    currentLine = tail;
                    currentColors = std::move(tailColors);
                    currentWidth -= widthAtBreak;
                } else {
                    // A single word longer than the line: fall back to char wrap.
                    pushLine(currentLine, currentColors, para.color, para.font, false, true);
                    currentLine.clear();
                    currentColors.clear();
                    currentWidth = 0.0f;
                }
                lastBreakPos = 0;
                widthAtBreak = 0.0f;
            }

            currentLine.append(s, len);
            const size_t sourceOffset = static_cast<size_t>(s - para.text.data());
            currentColors.insert(currentColors.end(), sourceColors.begin() + sourceOffset,
                                 sourceColors.begin() + sourceOffset + len);
            currentWidth += cw;
            if (cp == ' ' || cp == '\t') {
                lastBreakPos = (int)currentLine.size();
                widthAtBreak = currentWidth;
            }
            s += len;
        }

        // Final line of the paragraph carries a hard break; no trailing newline
        // for the very last paragraph so the buffer never ends on a blank line.
        pushLine(currentLine, currentColors, para.color, para.font, true, !lastPara);
    }

    IM_ASSERT(byteColors.size() == static_cast<size_t>(buf.size()));

    if (selectionStart > buf.size() || selectionEnd > buf.size()) {
        selectionStart = selectionEnd = -1;
        hasStoredSelection = false;
        isSelecting = false;
    }
}

int SelectableTextView::clampIndexToCodepointBoundary(int idx) const {
    idx = std::max(0, std::min(idx, (int)buf.size()));
    while (idx > 0) {
        unsigned char c = (unsigned char)buf[idx];
        if ((c & 0xC0) != 0x80) break;
        idx--;
    }
    return idx;
}

bool SelectableTextView::isWordChar(unsigned int cp) {
    if (cp == '_') return true;
    if (cp >= '0' && cp <= '9') return true;
    if (cp >= 'A' && cp <= 'Z') return true;
    if (cp >= 'a' && cp <= 'z') return true;
    if (cp > 127 && (ImCharIsBlankW((ImWchar)cp) == false)) return true;
    return false;
}

void SelectableTextView::selectWordAt(int bufferIndex) {
    if (buf.empty()) return;
    int n = (int)buf.size();
    int idx = clampIndexToCodepointBoundary(bufferIndex);
    if (idx >= n) idx = n - 1;
    if (idx < 0) return;

    int cp_start = clampIndexToCodepointBoundary(idx);
    const char* begin = buf.begin();
    unsigned int cp = 0;
    int len = ImTextCharFromUtf8(&cp, begin + cp_start, begin + n);
    if (len <= 0) {
        selectionStart = cp_start;
        selectionEnd = std::min(cp_start + 1, n);
        hasStoredSelection = selectionEnd > selectionStart;
        return;
    }

    bool wordChar = isWordChar(cp);

    int left = cp_start;
    while (left > 0) {
        int prev = left;
        do { prev--; } while (prev > 0 && ((unsigned char)buf[prev] & 0xC0) == 0x80);
        unsigned int cp2 = 0;
        int l2 = ImTextCharFromUtf8(&cp2, begin + prev, begin + n);
        if (l2 <= 0) break;
        if (isWordChar(cp2) != wordChar) break;
        left = prev;
    }

    int right = cp_start + len;
    while (right < n) {
        unsigned int cp2 = 0;
        int l2 = ImTextCharFromUtf8(&cp2, begin + right, begin + n);
        if (l2 <= 0) break;
        if (isWordChar(cp2) != wordChar) break;
        right += l2;
    }

    selectionStart = left;
    selectionEnd = right;
    hasStoredSelection = selectionEnd > selectionStart;
}

void SelectableTextView::selectLineAt(int bufferIndex) {
    if (buf.empty()) return;
    int n = (int)buf.size();
    int idx = std::max(0, std::min(bufferIndex, n));

    int start = idx;
    while (start > 0 && buf[start - 1] != '\n') start--;
    int end = idx;
    while (end < n && buf[end] != '\n') end++;

    selectionStart = start;
    selectionEnd = end;
    hasStoredSelection = selectionEnd > selectionStart;
}

void SelectableTextView::selectAll() {
    selectionStart = 0;
    selectionEnd = (int)buf.size();
    hasStoredSelection = selectionEnd > selectionStart;
}

std::string SelectableTextView::buildCopyText(int startInclusive, int endExclusive) const {
    int a = std::max(0, startInclusive);
    int b = std::min((int)buf.size(), endExclusive);
    if (b <= a) {
        return {};
    }

    const int totalLines = (lineOffsets.Size > 0) ? (lineOffsets.Size - 1) : 0;

    auto lineStartIndexFn = [&](int line) -> int {
        return (line >= 0 && line < lineOffsets.Size) ? lineOffsets[line] : 0;
    };
    auto lineEndIndexExclFn = [&](int line) -> int {
        if (line < 0 || line + 1 >= lineOffsets.Size) {
            return (int)buf.size();
        }
        int end = lineOffsets[line + 1];
        if (end > 0 && end <= (int)buf.size() && buf[end - 1] == '\n') {
            end -= 1;
        }
        return end;
    };
    auto idxToLine = [&](int idx) -> int {
        int l = 0;
        int r = (lineOffsets.Size > 0) ? lineOffsets.Size - 2 : -1;
        int ans = 0;
        while (l <= r) {
            int m = (l + r) >> 1;
            if (lineOffsets[m] <= idx) {
                ans = m;
                l = m + 1;
            } else {
                r = m - 1;
            }
        }
        return std::max(0, std::min(ans, totalLines > 0 ? totalLines - 1 : 0));
    };

    std::string out;
    out.reserve(static_cast<size_t>(b - a));
    if (totalLines > 0) {
        int startLine = idxToLine(a);
        int endLine = idxToLine(std::max(a, b - 1));
        for (int i = startLine; i <= endLine; ++i) {
            int ls = lineStartIndexFn(i);
            int le = lineEndIndexExclFn(i);

            int s0 = std::max(a, ls);
            int e0 = std::min(b, le);
            if (e0 > s0) {
                out.append(buf.begin() + s0, buf.begin() + e0);
            }

            if (i + 1 < lineOffsets.Size) {
                int nextStart = lineOffsets[i + 1];
                bool hadNewlineChar = (nextStart > 0 && nextStart <= (int)buf.size() &&
                                       buf[nextStart - 1] == '\n');
                int nlPos = hadNewlineChar ? nextStart - 1 : -1;
                if (hadNewlineChar && i < (int)lineHardBreak.size() && lineHardBreak[i] &&
                    nlPos >= a && nlPos < b) {
                    out.push_back('\n');
                }
            }
        }
    }

    return out;
}

bool SelectableTextView::copySelectionToClipboard() const {
    if (!hasStoredSelection) return false;
    int a = std::min(selectionStart, selectionEnd);
    int b = std::max(selectionStart, selectionEnd);
    std::string out = buildCopyText(a, b);
    if (out.empty()) return false;
    ImGui::SetClipboardText(out.c_str());
    return true;
}

void SelectableTextView::copyAllToClipboard() const {
    if (buf.empty()) return;
    std::string out = buildCopyText(0, (int)buf.size());
    if (!out.empty()) {
        ImGui::SetClipboardText(out.c_str());
    }
}

void SelectableTextView::draw(const char* id, const ImVec2& size,
                              const std::vector<Paragraph>& paragraphs, bool scrollToBottom,
                              float lineSpacingY) {
    if (!ImGui::BeginChild(id, size, 0,
                           ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        ImGui::EndChild();
        return;
    }

    float contentW = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    float wrapW = ImMax(1.0f, contentW);

    size_t hash = paragraphs.size();
    for (const Paragraph& p : paragraphs) {
        hash ^= std::hash<std::string>{}(p.text) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        hash ^= static_cast<size_t>(p.color) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        hash ^= reinterpret_cast<size_t>(p.font) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        hash ^= static_cast<size_t>(p.rule ? 1 : 0) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        for (const ColorSpan& span : p.colorSpans) {
            hash ^= static_cast<size_t>(span.begin) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
            hash ^= static_cast<size_t>(span.end) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
            hash ^= static_cast<size_t>(span.color) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        }
    }
    if (hash != builtHash || fabsf(builtWrapWidth - wrapW) > 0.5f) {
        rebuild(paragraphs, wrapW);
        builtHash = hash;
        builtWrapWidth = wrapW;
    }

    ImFont* font = ImGui::GetFont();
    const float fontSize = ImGui::GetFontSize();
    const float lineBoxHeight = ImGui::GetTextLineHeight();
    const float rowSpacing = (lineSpacingY >= 0.0f)
        ? lineSpacingY
        : ImGui::GetStyle().ItemSpacing.y;
    const float lineAdvance = lineBoxHeight + rowSpacing;
    const int totalLines = (lineOffsets.Size > 0) ? (lineOffsets.Size - 1) : 0;

    if (lineSpacingY >= 0.0f) {
        const ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, lineSpacingY));
    }

    const bool winFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
    const ImGuiIO& io = ImGui::GetIO();
    if (winFocused) {
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A, false)) selectAll();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C, false)) copySelectionToClipboard();
    }

    auto lineStartIndex = [&](int line) -> int {
        return (line >= 0 && line < lineOffsets.Size) ? lineOffsets[line] : 0;
    };
    auto lineEndIndexExcl = [&](int line) -> int {
        if (line < 0 || line + 1 >= lineOffsets.Size) return (int)buf.size();
        int end = lineOffsets[line + 1];
        if (end > 0 && end <= (int)buf.size() && buf[end - 1] == '\n') end -= 1;
        return end;
    };
    auto lineFontFor = [&](int line) -> ImFont* {
        ImFont* lf = (line >= 0 && line < (int)lineFonts.size()) ? lineFonts[line] : nullptr;
        return lf ? lf : font;
    };
    auto lineIsRule = [&](int line) -> bool {
        return line >= 0 && line < (int)lineRule.size() && lineRule[line];
    };
    // Rule lines center their text; every x computation on such a line must
    // account for this offset.
    auto lineTextOffsetX = [&](int line) -> float {
        if (!lineIsRule(line)) return 0.0f;
        int ls = lineStartIndex(line);
        int le = lineEndIndexExcl(line);
        if (ls >= le) return 0.0f;
        float textW = lineFontFor(line)->CalcTextSizeA(fontSize, FLT_MAX, 0.0f,
                                                       buf.begin() + ls, buf.begin() + le).x;
        return ImMax(0.0f, (wrapW - textW) * 0.5f);
    };

    int hoveredLine = -1;
    float hoveredLocalX = 0.0f;

    ImGuiListClipper clipper;
    clipper.Begin(totalLines, lineAdvance);
    const ImU32 selBg = ImGui::GetColorU32(ImGuiCol_TextSelectedBg);

    int selA = selectionStart, selB = selectionEnd;
    if (selA > selB) std::swap(selA, selB);

    while (clipper.Step()) {
        for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line) {
            int ls = lineStartIndex(line);
            int le = lineEndIndexExcl(line);
            ImVec2 linePos = ImGui::GetCursorScreenPos();

            ImRect hitRect(ImVec2(linePos.x, linePos.y),
                           ImVec2(linePos.x + wrapW, linePos.y + lineAdvance));
            if (hoveredLine == -1 && ImGui::IsMouseHoveringRect(hitRect.Min, hitRect.Max)) {
                hoveredLine = line;
                hoveredLocalX = ImGui::GetMousePos().x - linePos.x;
            }

            ImFont* lineFont = lineFontFor(line);
            const bool customFont = (lineFont != font);
            const float textOffsetX = lineTextOffsetX(line);

            if (hasStoredSelection && selA >= 0 && selB > selA) {
                int hs = std::max(ls, selA);
                int he = std::min(le, selB);
                if (hs < he) {
                    const char* s0 = buf.begin() + ls;
                    const char* s1 = buf.begin() + hs;
                    const char* s2 = buf.begin() + he;
                    float x0 = lineFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, s0, s1).x;
                    float x1 = lineFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, s1, s2).x;
                    ImGui::GetWindowDrawList()->AddRectFilled(
                        ImVec2(linePos.x + textOffsetX + x0, linePos.y),
                        ImVec2(linePos.x + textOffsetX + x0 + x1, linePos.y + lineBoxHeight),
                        selBg);
                }
            }

            ImU32 col = (line >= 0 && line < (int)lineColors.size())
                ? lineColors[line] : ImGui::GetColorU32(ImGuiCol_Text);

            if (lineIsRule(line) && ls < le) {
                float textW = lineFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f,
                                                      buf.begin() + ls, buf.begin() + le).x;
                const float gap = fontSize * 0.5f;
                const float midY = linePos.y + lineBoxHeight * 0.5f;
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                if (textOffsetX > gap) {
                    drawList->AddLine(ImVec2(linePos.x, midY),
                                      ImVec2(linePos.x + textOffsetX - gap, midY), col);
                }
                float rightStart = textOffsetX + textW + gap;
                if (rightStart < wrapW) {
                    drawList->AddLine(ImVec2(linePos.x + rightStart, midY),
                                      ImVec2(linePos.x + wrapW, midY), col);
                }
            }

            if (textOffsetX > 0.0f) {
                ImGui::SetCursorScreenPos(ImVec2(linePos.x + textOffsetX, linePos.y));
            }
            bool hasInlineColor = false;
            for (int i = ls; i < le; ++i) {
                if (byteColors[static_cast<size_t>(i)] != col) {
                    hasInlineColor = true;
                    break;
                }
            }

            if (hasInlineColor && ls < le) {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                float x = linePos.x + textOffsetX;
                int runStart = ls;
                while (runStart < le) {
                    const ImU32 runColor = byteColors[static_cast<size_t>(runStart)];
                    int runEnd = runStart + 1;
                    while (runEnd < le &&
                           byteColors[static_cast<size_t>(runEnd)] == runColor) {
                        ++runEnd;
                    }
                    drawList->AddText(lineFont, fontSize, ImVec2(x, linePos.y), runColor,
                                      buf.begin() + runStart, buf.begin() + runEnd);
                    x += lineFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f,
                                                 buf.begin() + runStart,
                                                 buf.begin() + runEnd).x;
                    runStart = runEnd;
                }
                ImGui::Dummy(ImVec2(x - linePos.x, lineBoxHeight));
            } else {
                if (customFont) ImGui::PushFont(lineFont, fontSize);
                ImGui::PushStyleColor(ImGuiCol_Text, col);
                if (ls < le) {
                    ImGui::TextUnformatted(buf.begin() + ls, buf.begin() + le);
                } else {
                    ImGui::TextUnformatted("");
                }
                ImGui::PopStyleColor();
                if (customFont) ImGui::PopFont();
            }
        }
    }
    clipper.End();

    auto indexFromLineAndLocalX = [&](int line, float localX) -> int {
        if (line < 0 || totalLines <= 0) return 0;
        int ls = lineStartIndex(line);
        int le = lineEndIndexExcl(line);
        if (ls >= le) return ls;
        localX -= lineTextOffsetX(line);
        ImFont* lineFont = lineFontFor(line);
        const char* s = buf.begin() + ls;
        const char* e = buf.begin() + le;
        float x = 0.0f;
        int idx = ls;
        for (const char* p = s; p < e;) {
            unsigned int c;
            int len = ImTextCharFromUtf8(&c, p, e);
            if (len <= 0) break;
            float cw = lineFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, p, p + len).x;
            if (localX < x + cw * 0.5f) return idx;
            x += cw;
            p += len;
            idx += len;
        }
        return le;
    };

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)) {
        if (hoveredLine >= 0) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hoveredLine >= 0) {
            int idx = indexFromLineAndLocalX(hoveredLine, hoveredLocalX);
            int clicks = ImGui::GetIO().MouseClickedCount[ImGuiMouseButton_Left];
            if (clicks >= 3) {
                selectLineAt(idx);
                isSelecting = false;
            } else if (clicks == 2) {
                selectWordAt(idx);
                isSelecting = false;
            } else {
                selectionStart = selectionEnd = idx;
                hasStoredSelection = true;
                isSelecting = true;
            }
        } else if (isSelecting && ImGui::IsMouseDown(ImGuiMouseButton_Left) && hoveredLine >= 0) {
            int idx = indexFromLineAndLocalX(hoveredLine, hoveredLocalX);
            selectionEnd = idx;
            hasStoredSelection = (selectionStart != selectionEnd);
        }
    }
    if (isSelecting && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        isSelecting = false;
    }

    if (lineSpacingY >= 0.0f) {
        ImGui::PopStyleVar();
    }

    if (ImGui::BeginPopupContextWindow("##sel_ctx", ImGuiPopupFlags_MouseButtonRight)) {
        const bool hasContent = !buf.empty();
        int a = std::min(selectionStart, selectionEnd);
        int b = std::max(selectionStart, selectionEnd);
        const bool canCopy = hasStoredSelection && b > a;
        if (ImGui::MenuItem(ICON_FA_COPY " Copy", "Ctrl+C", false, canCopy)) {
            copySelectionToClipboard();
        }
        if (ImGui::MenuItem(ICON_FA_CLIPBOARD_LIST " Copy All", nullptr, false, hasContent)) {
            copyAllToClipboard();
        }
        if (ImGui::MenuItem(ICON_FA_I_CURSOR " Select All", "Ctrl+A", false, hasContent)) {
            selectAll();
        }
        ImGui::EndPopup();
    }

    // Sticky auto-scroll: stay pinned to the bottom across content growth, and
    // release only when the user scrolls up. Re-pinning every frame converges
    // even when a large message is appended in a single frame.
    float currentScrollY = ImGui::GetScrollY();
    float maxScrollY = ImGui::GetScrollMaxY();
    if (!isSelecting && fabsf(currentScrollY - lastScrollY) > 0.5f) {
        stickToBottom = (currentScrollY >= maxScrollY - 1.0f);
    }
    if (scrollToBottom) {
        stickToBottom = true;
    }
    if (stickToBottom && !isSelecting) {
        ImGui::SetScrollY(maxScrollY);
        currentScrollY = maxScrollY;
    }
    lastScrollY = currentScrollY;

    ImGui::EndChild();
}

} // namespace doriax::editor
