// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Input.h"

#include <string>

namespace doriax::editor{

    // Raw device control that drives one gamepad button or axis
    struct GamepadInput{
        enum class Source{
            NONE,
            AXIS,
            BUTTON,
            HAT
        };

        Source source = Source::NONE;
        int index = 0;
        int hatMask = 0;
        // Raw axis value is scaled and offset into [-1, 1], negative scale inverts
        float scale = 1.0f;
        float offset = 0.0f;
    };

    // Device layout from SDL_GameControllerDB, in engine (D_GAMEPAD_*) order
    struct GamepadMapping{
        GamepadInput buttons[D_GAMEPAD_BUTTON_LAST + 1];
        GamepadInput axes[D_GAMEPAD_AXIS_LAST + 1];
    };

    // Kernel layouts disagree on which axes are the right stick and which are the
    // triggers, so the device GUID decides. False when the database has no entry.
    bool findGamepadMapping(const std::string& guid, GamepadMapping& mapping);

}
