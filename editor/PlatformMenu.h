// (c) Eduardo Doria
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace doriax::editor {

enum class PlatformMenuItemType {
    Command,
    Separator,
    Submenu
};

// Native backends can place these commands in the application's standard menu.
enum class PlatformMenuRole { None, About, Settings, Quit };

struct PlatformMenuCommand {
    uint32_t id = 0;
    std::string payload;

    bool operator==(const PlatformMenuCommand& other) const {
        return id == other.id && payload == other.payload;
    }
};

struct PlatformMenuItem {
    PlatformMenuItemType type = PlatformMenuItemType::Command;
    PlatformMenuRole role = PlatformMenuRole::None;
    std::string label;
    std::string shortcut;
    PlatformMenuCommand command;
    bool enabled = true;
    bool checked = false;
    std::vector<PlatformMenuItem> children;

    bool operator==(const PlatformMenuItem& other) const {
        return type == other.type
            && role == other.role
            && label == other.label
            && shortcut == other.shortcut
            && command == other.command
            && enabled == other.enabled
            && checked == other.checked
            && children == other.children;
    }
};

struct PlatformMenuModel {
    std::vector<PlatformMenuItem> menus;
};

using PlatformMenuCallback = std::function<void(const PlatformMenuCommand&)>;

} // namespace doriax::editor
