// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "GamepadWin.h"

#include "Engine.h"

#include <cmath>

using namespace doriax;

namespace {

constexpr ULONGLONG SCAN_INTERVAL_MS = 1000;

float normalizeThumb(SHORT value) {
    return value >= 0 ? static_cast<float>(value) / 32767.0f
                      : static_cast<float>(value) / 32768.0f;
}

}

void GamepadWin::init() {
    constexpr const wchar_t* libraries[] = {
        L"xinput1_4.dll", L"xinput1_3.dll", L"xinput9_1_0.dll"
    };
    for (const wchar_t* name : libraries) {
        library = LoadLibraryW(name);
        if (!library) continue;
        getState = reinterpret_cast<GetStateProc>(
            GetProcAddress(library, "XInputGetState"));
        if (getState) return;
        FreeLibrary(library);
        library = nullptr;
    }
}

void GamepadWin::shutdown() {
    for (int id = 0; id < COUNT; ++id) disconnect(id);
    if (library) FreeLibrary(library);
    library = nullptr;
    getState = nullptr;
}

void GamepadWin::disconnect(int id) {
    Pad& pad = pads[id];
    if (!pad.connected) return;
    pad = {};
    Engine::systemGamepadDisconnect(id);
}

void GamepadWin::poll() {
    if (!getState) return;

    // Probing an empty slot costs a device query on the older XInput libraries,
    // so look for new controllers once per second instead of every frame.
    const ULONGLONG now = GetTickCount64();
    const bool scanEmptySlots = now >= nextScan;
    if (scanEmptySlots) nextScan = now + SCAN_INTERVAL_MS;

    constexpr std::array<WORD, BUTTON_COUNT> buttonMasks = {
        XINPUT_GAMEPAD_A,
        XINPUT_GAMEPAD_B,
        XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_LEFT_SHOULDER,
        XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_BACK,
        XINPUT_GAMEPAD_START,
        0, // guide, not reported by the public XInput API
        XINPUT_GAMEPAD_LEFT_THUMB,
        XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_DPAD_UP,
        XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_DPAD_DOWN,
        XINPUT_GAMEPAD_DPAD_LEFT
    };

    for (DWORD id = 0; id < COUNT; ++id) {
        Pad& pad = pads[id];
        if (!pad.connected && !scanEmptySlots) continue;

        XINPUT_STATE state{};
        const bool connected = getState(id, &state) == ERROR_SUCCESS;
        if (!connected) {
            disconnect(static_cast<int>(id));
            continue;
        }
        if (!pad.connected) {
            pad = {};
            pad.connected = true;
            // triggers rest at -1; seed so a resting trigger doesn't emit a
            // spurious axis move from 0 to -1 on connect
            pad.axes[4] = pad.axes[5] = -1.0f;
            Engine::systemGamepadConnect(static_cast<int>(id), "XInput Controller");
        }

        for (int button = 0; button < BUTTON_COUNT; ++button) {
            const unsigned char pressed = buttonMasks[button] != 0 &&
                (state.Gamepad.wButtons & buttonMasks[button]) != 0;
            if (pressed == pad.buttons[button]) continue;
            pad.buttons[button] = pressed;
            if (pressed) Engine::systemGamepadButtonDown(id, button);
            else Engine::systemGamepadButtonUp(id, button);
        }

        const std::array<float, AXIS_COUNT> axes = {
            normalizeThumb(state.Gamepad.sThumbLX),
            -normalizeThumb(state.Gamepad.sThumbLY),
            normalizeThumb(state.Gamepad.sThumbRX),
            -normalizeThumb(state.Gamepad.sThumbRY),
            static_cast<float>(state.Gamepad.bLeftTrigger) / 127.5f - 1.0f,
            static_cast<float>(state.Gamepad.bRightTrigger) / 127.5f - 1.0f
        };
        for (int axis = 0; axis < AXIS_COUNT; ++axis) {
            if (std::fabs(axes[axis] - pad.axes[axis]) <= 0.001f) continue;
            pad.axes[axis] = axes[axis];
            Engine::systemGamepadAxisMove(id, axis, axes[axis]);
        }
    }
}
