// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "GamepadLinux.h"

#include "Engine.h"

#include <fcntl.h>
#include <glob.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace doriax;

namespace {

int buttonForCode(unsigned short code) {
    if (code == BTN_SOUTH) return 0;
    if (code == BTN_EAST) return 1;
    if (code == BTN_WEST) return 2;
    if (code == BTN_NORTH) return 3;
    if (code == BTN_TL) return 4;
    if (code == BTN_TR) return 5;
    if (code == BTN_SELECT) return 6;
    if (code == BTN_START) return 7;
    if (code == BTN_MODE) return 8;
    if (code == BTN_THUMBL) return 9;
    if (code == BTN_THUMBR) return 10;
    if (code == BTN_DPAD_UP) return 11;
    if (code == BTN_DPAD_RIGHT) return 12;
    if (code == BTN_DPAD_DOWN) return 13;
    if (code == BTN_DPAD_LEFT) return 14;
    return -1;
}

int axisForCode(unsigned char code) {
    switch (code) {
        case ABS_X: return 0;
        case ABS_Y: return 1;
        case ABS_RX: return 2;
        case ABS_RY: return 3;
        case ABS_Z: case ABS_BRAKE: return 4;
        case ABS_RZ: case ABS_GAS: return 5;
        default: return -1;
    }
}

// Same identifier the community database is keyed by, see SDL_GameControllerDB
bool readGuid(int id, std::string& guid) {
    static const char* const parts[] = {"bustype", "vendor", "product", "version"};
    unsigned int values[4] = {};
    for (int i = 0; i < 4; ++i) {
        char path[128];
        std::snprintf(path, sizeof(path),
                      "/sys/class/input/js%d/device/id/%s", id, parts[i]);
        FILE* file = std::fopen(path, "r");
        if (!file) return false;
        const int read = std::fscanf(file, "%x", &values[i]);
        std::fclose(file);
        if (read != 1) return false;
    }
    if (!values[1] || !values[2] || !values[3]) return false;

    char text[33];
    std::snprintf(text, sizeof(text),
                  "%02x%02x0000%02x%02x0000%02x%02x0000%02x%02x0000",
                  values[0] & 0xff, (values[0] >> 8) & 0xff,
                  values[1] & 0xff, (values[1] >> 8) & 0xff,
                  values[2] & 0xff, (values[2] >> 8) & 0xff,
                  values[3] & 0xff, (values[3] >> 8) & 0xff);
    guid = text;
    return true;
}

}

void GamepadLinux::setButton(int id, Pad& pad, int button, bool pressed) {
    if (button < 0 || button >= BUTTON_COUNT ||
        pad.buttons[button] == static_cast<unsigned char>(pressed))
        return;
    pad.buttons[button] = static_cast<unsigned char>(pressed);
    if (pressed) Engine::systemGamepadButtonDown(id, button);
    else Engine::systemGamepadButtonUp(id, button);
}

void GamepadLinux::setAxis(int id, Pad& pad, int axis, float value) {
    if (axis < 0 || axis >= AXIS_COUNT) return;
    if (std::fabs(value - pad.axes[axis]) <= 0.001f) return;
    pad.axes[axis] = value;
    Engine::systemGamepadAxisMove(id, axis, value);
}

void GamepadLinux::updateHat(int id, Pad& pad, bool horizontal, int value) {
    if (horizontal) pad.hatX = value;
    else pad.hatY = value;
    // Community mappings address the hat as the bits of h0
    pad.hat = (pad.hatY < 0 ? 1 : 0) | (pad.hatX > 0 ? 2 : 0) |
              (pad.hatY > 0 ? 4 : 0) | (pad.hatX < 0 ? 8 : 0);
    if (pad.mapped) return;
    setButton(id, pad, 11, pad.hatY < 0);
    setButton(id, pad, 12, pad.hatX > 0);
    setButton(id, pad, 13, pad.hatY > 0);
    setButton(id, pad, 14, pad.hatX < 0);
}

void GamepadLinux::applyMapping(int id, Pad& pad) {
    auto axisValue = [&](const editor::GamepadInput& input) -> float {
        if (input.source == editor::GamepadInput::Source::AXIS &&
            input.index < RAW_AXIS_COUNT) {
            return std::clamp(
                pad.rawAxes[input.index] * input.scale + input.offset, -1.0f, 1.0f);
        }
        if (input.source == editor::GamepadInput::Source::BUTTON &&
            input.index < RAW_BUTTON_COUNT) {
            return pad.rawButtons[input.index] ? 1.0f : -1.0f;
        }
        return -1.0f;
    };

    auto buttonValue = [&](const editor::GamepadInput& input) -> bool {
        if (input.source == editor::GamepadInput::Source::BUTTON)
            return input.index < RAW_BUTTON_COUNT && pad.rawButtons[input.index];
        if (input.source == editor::GamepadInput::Source::HAT)
            return input.index == 0 && (pad.hat & input.hatMask) != 0;
        if (input.source == editor::GamepadInput::Source::AXIS &&
            input.index < RAW_AXIS_COUNT) {
            const float value = pad.rawAxes[input.index] * input.scale + input.offset;
            if (input.offset < 0.0f || (input.offset == 0.0f && input.scale > 0.0f))
                return value >= 0.0f;
            return value <= 0.0f;
        }
        return false;
    };

    for (int button = 0; button < BUTTON_COUNT; ++button) {
        const editor::GamepadInput& input = pad.mapping.buttons[button];
        if (input.source != editor::GamepadInput::Source::NONE)
            setButton(id, pad, button, buttonValue(input));
    }
    for (int axis = 0; axis < AXIS_COUNT; ++axis) {
        const editor::GamepadInput& input = pad.mapping.axes[axis];
        if (input.source != editor::GamepadInput::Source::NONE)
            setAxis(id, pad, axis, axisValue(input));
    }
}

void GamepadLinux::connect(int id, const char* path) {
    Pad& pad = pads[id];
    const int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0) return;

    pad = Pad();
    pad.fd = fd;
    pad.connected = true;
    // triggers rest at -1; seed so a resting trigger doesn't emit a spurious
    // axis move from 0 to -1 on connect
    pad.axes[4] = pad.axes[5] = -1.0f;
    ioctl(fd, JSIOCGAXMAP, pad.axisMap.data());
    ioctl(fd, JSIOCGBTNMAP, pad.buttonMap.data());

    char name[128] = "Gamepad";
    if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) >= 0)
        name[sizeof(name) - 1] = '\0';
    pad.name = name;

    // The joystick device sends the current state of every control on open, so
    // the mapped values settle on the first poll
    std::string guid;
    pad.mapped = readGuid(id, guid) && editor::findGamepadMapping(guid, pad.mapping);

    Engine::systemGamepadConnect(id, pad.name);
}

void GamepadLinux::disconnect(int id) {
    Pad& pad = pads[id];
    if (!pad.connected) return;
    if (pad.fd >= 0) close(pad.fd);
    pad = Pad();
    Engine::systemGamepadDisconnect(id);
}

void GamepadLinux::scan(double now) {
    if (now < nextScan) return;
    nextScan = now + 1.0;

    glob_t paths{};
    if (glob("/dev/input/js*", 0, nullptr, &paths) == 0) {
        for (size_t i = 0; i < paths.gl_pathc; ++i) {
            const char* path = paths.gl_pathv[i];
            const char* number = path + std::strlen(path);
            while (number > path && number[-1] >= '0' && number[-1] <= '9') --number;
            const int id = std::atoi(number);
            if (id >= 0 && id < COUNT && !pads[id].connected)
                connect(id, path);
        }
    }
    globfree(&paths);
}

void GamepadLinux::readEvents(int id, Pad& pad) {
    js_event event{};
    while (read(pad.fd, &event, sizeof(event)) == sizeof(event)) {
        const unsigned char type = event.type & ~JS_EVENT_INIT;
        if (type == JS_EVENT_BUTTON) {
            if (event.number < RAW_BUTTON_COUNT)
                pad.rawButtons[event.number] = event.value ? 1 : 0;
            if (!pad.mapped && event.number < pad.buttonMap.size()) {
                // Without a database entry the kernel's own codes are all there
                // is to go on. The index is not the code: a pad that reports
                // SOUTH/EAST/NORTH/WEST skips BTN_C, so index arithmetic from
                // BTN_SOUTH lands on the wrong face buttons.
                setButton(id, pad, buttonForCode(pad.buttonMap[event.number]),
                          event.value != 0);
            }
        } else if (type == JS_EVENT_AXIS) {
            const unsigned char code = event.number < ABS_CNT
                ? pad.axisMap[event.number] : 0;
            if (code >= ABS_HAT0X && code <= ABS_HAT3Y) {
                updateHat(id, pad, ((code - ABS_HAT0X) % 2) == 0, event.value);
            } else {
                const float value = event.value / 32767.0f;
                if (event.number < RAW_AXIS_COUNT)
                    pad.rawAxes[event.number] = value;
                if (!pad.mapped)
                    setAxis(id, pad, axisForCode(code), value);
            }
        }
    }

    // A disconnected device reports an error other than "nothing to read"
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
        disconnect(id);
        return;
    }

    if (pad.mapped) applyMapping(id, pad);
}

void GamepadLinux::poll(double now) {
    scan(now);
    for (int id = 0; id < COUNT; ++id) {
        Pad& pad = pads[id];
        if (!pad.connected) continue;
        readEvents(id, pad);
    }
}

void GamepadLinux::shutdown() {
    for (int id = 0; id < COUNT; ++id) disconnect(id);
}
