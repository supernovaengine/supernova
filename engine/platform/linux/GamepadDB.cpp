// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "GamepadDB.h"

#include "GamepadMappings.h"

#include <cstdlib>
#include <cstring>

using namespace doriax;
using namespace doriax::editor;

namespace {

constexpr size_t GUID_LENGTH = 32;

int buttonForName(const std::string& name){
    if (name == "a") return D_GAMEPAD_BUTTON_A;
    if (name == "b") return D_GAMEPAD_BUTTON_B;
    if (name == "x") return D_GAMEPAD_BUTTON_X;
    if (name == "y") return D_GAMEPAD_BUTTON_Y;
    if (name == "leftshoulder") return D_GAMEPAD_BUTTON_LEFT_BUMPER;
    if (name == "rightshoulder") return D_GAMEPAD_BUTTON_RIGHT_BUMPER;
    if (name == "back") return D_GAMEPAD_BUTTON_BACK;
    if (name == "start") return D_GAMEPAD_BUTTON_START;
    if (name == "guide") return D_GAMEPAD_BUTTON_GUIDE;
    if (name == "leftstick") return D_GAMEPAD_BUTTON_LEFT_THUMB;
    if (name == "rightstick") return D_GAMEPAD_BUTTON_RIGHT_THUMB;
    if (name == "dpup") return D_GAMEPAD_BUTTON_DPAD_UP;
    if (name == "dpright") return D_GAMEPAD_BUTTON_DPAD_RIGHT;
    if (name == "dpdown") return D_GAMEPAD_BUTTON_DPAD_DOWN;
    if (name == "dpleft") return D_GAMEPAD_BUTTON_DPAD_LEFT;
    return -1;
}

int axisForName(const std::string& name){
    if (name == "leftx") return D_GAMEPAD_AXIS_LEFT_X;
    if (name == "lefty") return D_GAMEPAD_AXIS_LEFT_Y;
    if (name == "rightx") return D_GAMEPAD_AXIS_RIGHT_X;
    if (name == "righty") return D_GAMEPAD_AXIS_RIGHT_Y;
    if (name == "lefttrigger") return D_GAMEPAD_AXIS_LEFT_TRIGGER;
    if (name == "righttrigger") return D_GAMEPAD_AXIS_RIGHT_TRIGGER;
    return -1;
}

// Source syntax: bN, aN, +aN, -aN, aN~ and hN.mask
bool parseInput(const std::string& text, GamepadInput& input){
    if (text.empty()) return false;

    size_t pos = 0;
    float minimum = -1.0f;
    float maximum = 1.0f;
    if (text[pos] == '+'){
        minimum = 0.0f;
        pos++;
    }else if (text[pos] == '-'){
        maximum = 0.0f;
        pos++;
    }
    if (pos >= text.size()) return false;

    const char kind = text[pos++];
    const char* digits = text.c_str() + pos;
    char* end = nullptr;
    const long index = std::strtol(digits, &end, 10);
    if (end == digits || index < 0) return false;
    pos += end - digits;

    input.index = static_cast<int>(index);
    if (kind == 'b'){
        input.source = GamepadInput::Source::BUTTON;
    }else if (kind == 'h'){
        if (pos >= text.size() || text[pos] != '.') return false;
        input.source = GamepadInput::Source::HAT;
        input.hatMask = std::atoi(text.c_str() + pos + 1);
    }else if (kind == 'a'){
        input.source = GamepadInput::Source::AXIS;
        input.scale = 2.0f / (maximum - minimum);
        input.offset = -(maximum + minimum) / (maximum - minimum);
        if (pos < text.size() && text[pos] == '~'){
            input.scale = -input.scale;
            input.offset = -input.offset;
        }
    }else{
        return false;
    }

    return true;
}

bool parseMapping(const char* entry, GamepadMapping& mapping){
    GamepadMapping parsed;
    bool isLinux = false;

    const std::string text = entry;
    size_t start = text.find(',');
    if (start == std::string::npos) return false;
    start = text.find(',', start + 1);

    while (start != std::string::npos){
        const size_t end = text.find(',', start + 1);
        const std::string field = text.substr(start + 1, end - start - 1);
        start = end;

        const size_t separator = field.find(':');
        if (separator == std::string::npos) continue;
        const std::string name = field.substr(0, separator);
        const std::string value = field.substr(separator + 1);

        if (name == "platform"){
            isLinux = value == "Linux";
            continue;
        }

        GamepadInput input;
        if (!parseInput(value, input)) continue;

        const int button = buttonForName(name);
        if (button >= 0){
            parsed.buttons[button] = input;
            continue;
        }
        const int axis = axisForName(name);
        if (axis >= 0) parsed.axes[axis] = input;
    }

    if (!isLinux) return false;
    mapping = parsed;

    return true;
}

}

bool doriax::editor::findGamepadMapping(const std::string& guid, GamepadMapping& mapping){
    if (guid.size() != GUID_LENGTH) return false;

    for (size_t i = 0; i < DORIAX_GAMEPAD_MAPPINGS_COUNT; i++){
        const char* entry = DORIAX_GAMEPAD_MAPPINGS[i];
        if (std::strncmp(entry, guid.c_str(), GUID_LENGTH) != 0) continue;
        if (entry[GUID_LENGTH] != ',') continue;
        if (parseMapping(entry, mapping)) return true;
    }

    return false;
}
