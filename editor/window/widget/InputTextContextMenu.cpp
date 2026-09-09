// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "InputTextContextMenu.h"

#include "external/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cstring>

namespace doriax::editor {

namespace {

int boundedTextLength(const char* buffer, size_t bufferSize) {
    if (!buffer || bufferSize == 0) {
        return 0;
    }
    const void* end = std::memchr(buffer, '\0', bufferSize);
    return end ? static_cast<int>(static_cast<const char*>(end) - buffer)
               : static_cast<int>(bufferSize - 1);
}

int validUtf8PrefixLength(const char* text, int textLen, int maxBytes) {
    int end = std::min(textLen, maxBytes);
    int valid = 0;
    while (valid < end) {
        unsigned int cp = 0;
        int len = ImTextCharFromUtf8(&cp, text + valid, text + textLen);
        if (len <= 0) {
            len = 1;
        }
        if (valid + len > end) {
            break;
        }
        valid += len;
    }
    return valid;
}

std::string defaultCopyRange(const char* buffer, int start, int end) {
    if (!buffer || end <= start) {
        return {};
    }
    return std::string(buffer + start, buffer + end);
}

int replaceRange(char* buffer, size_t bufferSize, int start, int end, const char* insertText) {
    if (!buffer || bufferSize == 0) {
        return 0;
    }

    int len = boundedTextLength(buffer, bufferSize);
    start = std::clamp(start, 0, len);
    end = std::clamp(end, start, len);

    const char* insert = insertText ? insertText : "";
    int insertLen = static_cast<int>(std::strlen(insert));
    int preservedLen = len - (end - start);
    int maxInsertLen = std::max(0, static_cast<int>(bufferSize) - 1 - preservedLen);
    insertLen = validUtf8PrefixLength(insert, insertLen, maxInsertLen);

    if (end < len) {
        std::memmove(buffer + start + insertLen, buffer + end,
                     static_cast<size_t>(len - end + 1));
    } else {
        buffer[start + insertLen] = '\0';
    }
    if (insertLen > 0) {
        std::memcpy(buffer + start, insert, static_cast<size_t>(insertLen));
    }
    return insertLen;
}

void scheduleInputReload(ImGuiInputTextState* state, int selectionStart, int selectionEnd) {
    if (!state) {
        return;
    }
    state->WantReloadUserBuf = true;
    state->ReloadSelectionStart = selectionStart;
    state->ReloadSelectionEnd = selectionEnd;
    state->CursorFollow = true;
    state->CursorAnimReset();
}

} // namespace

bool InputTextContextMenu::drawForLastItem(char* buffer, size_t bufferSize) {
    return drawForLastItem(buffer, bufferSize, Options{});
}

bool InputTextContextMenu::drawForLastItem(char* buffer, size_t bufferSize,
                                           const Options& options) {
    if (!buffer || bufferSize == 0 || ImGui::GetItemID() == 0) {
        return false;
    }

    ImGuiID inputId = ImGui::GetItemID();
    ImGuiInputTextState* state = ImGui::GetInputTextState(inputId);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
        ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    bool changed = false;
    if (!ImGui::BeginPopupContextItem(options.popupId, ImGuiPopupFlags_MouseButtonRight)) {
        return false;
    }

    state = ImGui::GetInputTextState(inputId);
    const int textLen = boundedTextLength(buffer, bufferSize);
    const bool hasText = textLen > 0;
    const bool hasSelection = state && state->HasSelection();
    int selectionStart = hasSelection ? std::min(state->GetSelectionStart(), state->GetSelectionEnd()) : 0;
    int selectionEnd = hasSelection ? std::max(state->GetSelectionStart(), state->GetSelectionEnd()) : 0;
    selectionStart = std::clamp(selectionStart, 0, textLen);
    selectionEnd = std::clamp(selectionEnd, selectionStart, textLen);

    auto copySelection = [&]() -> std::string {
        if (!hasSelection || selectionEnd <= selectionStart) {
            return {};
        }
        if (options.copyRange) {
            return options.copyRange(buffer, selectionStart, selectionEnd, options.userData);
        }
        return defaultCopyRange(buffer, selectionStart, selectionEnd);
    };

    auto applyEdit = [&](int replaceStart, int replaceEnd, const char* insertText) {
        int insertedLen = replaceRange(buffer, bufferSize, replaceStart, replaceEnd, insertText);
        int cursorPos = replaceStart + insertedLen;
        int newSelectionStart = cursorPos;
        int newSelectionEnd = cursorPos;
        if (options.afterEdit) {
            options.afterEdit(buffer, bufferSize, &cursorPos,
                              &newSelectionStart, &newSelectionEnd, options.userData);
        }
        scheduleInputReload(state, newSelectionStart, newSelectionEnd);
        changed = true;
    };

    const bool canCopy = !options.password && hasSelection && selectionEnd > selectionStart;
    const bool canCut = !options.readOnly && canCopy;
    const bool canPaste = !options.readOnly && ImGui::GetClipboardText() != nullptr;
    const bool canSelectAll = state && hasText;

    if (ImGui::MenuItem(ICON_FA_COPY " Copy", "Ctrl+C", false, canCopy)) {
        std::string text = copySelection();
        if (!text.empty()) {
            ImGui::SetClipboardText(text.c_str());
        }
    }
    if (ImGui::MenuItem(ICON_FA_PASTE " Paste", "Ctrl+V", false, canPaste)) {
        const char* clipboard = ImGui::GetClipboardText();
        int pasteStart = hasSelection ? selectionStart : (state ? state->GetCursorPos() : textLen);
        int pasteEnd = hasSelection ? selectionEnd : pasteStart;
        pasteStart = std::clamp(pasteStart, 0, textLen);
        pasteEnd = std::clamp(pasteEnd, pasteStart, textLen);
        applyEdit(pasteStart, pasteEnd, clipboard ? clipboard : "");
    }
    if (ImGui::MenuItem(ICON_FA_SCISSORS " Cut", "Ctrl+X", false, canCut)) {
        std::string text = copySelection();
        if (!text.empty()) {
            ImGui::SetClipboardText(text.c_str());
        }
        applyEdit(selectionStart, selectionEnd, "");
    }
    ImGui::Separator();
    if (ImGui::MenuItem(ICON_FA_I_CURSOR " Select All", "Ctrl+A", false, canSelectAll)) {
        state->SelectAll();
        state->CursorFollow = true;
        state->CursorAnimReset();
    }

    ImGui::EndPopup();
    return changed;
}

} // namespace doriax::editor
