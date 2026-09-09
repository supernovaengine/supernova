// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// macOS virtual keycode -> engine key translation. The engine's key constants
// are GLFW's (see core/Input.h). Virtual keycodes are positional, so W/A/S/D
// stay on the same physical keys across keyboard layouts.

#ifndef KeyCodesMac_h
#define KeyCodesMac_h

#import <Cocoa/Cocoa.h>

#include "Input.h"

namespace doriax {

    inline const short* macKeycodeTable() {
        static short keys[256];
        static bool initialized = false;
        if (initialized) return keys;

        for (int i = 0; i < 256; ++i) keys[i] = D_KEY_UNKNOWN;

        keys[0x1D] = D_KEY_0;  keys[0x12] = D_KEY_1;  keys[0x13] = D_KEY_2;
        keys[0x14] = D_KEY_3;  keys[0x15] = D_KEY_4;  keys[0x17] = D_KEY_5;
        keys[0x16] = D_KEY_6;  keys[0x1A] = D_KEY_7;  keys[0x1C] = D_KEY_8;
        keys[0x19] = D_KEY_9;

        keys[0x00] = D_KEY_A;  keys[0x0B] = D_KEY_B;  keys[0x08] = D_KEY_C;
        keys[0x02] = D_KEY_D;  keys[0x0E] = D_KEY_E;  keys[0x03] = D_KEY_F;
        keys[0x05] = D_KEY_G;  keys[0x04] = D_KEY_H;  keys[0x22] = D_KEY_I;
        keys[0x26] = D_KEY_J;  keys[0x28] = D_KEY_K;  keys[0x25] = D_KEY_L;
        keys[0x2E] = D_KEY_M;  keys[0x2D] = D_KEY_N;  keys[0x1F] = D_KEY_O;
        keys[0x23] = D_KEY_P;  keys[0x0C] = D_KEY_Q;  keys[0x0F] = D_KEY_R;
        keys[0x01] = D_KEY_S;  keys[0x11] = D_KEY_T;  keys[0x20] = D_KEY_U;
        keys[0x09] = D_KEY_V;  keys[0x0D] = D_KEY_W;  keys[0x07] = D_KEY_X;
        keys[0x10] = D_KEY_Y;  keys[0x06] = D_KEY_Z;

        keys[0x27] = D_KEY_APOSTROPHE;
        keys[0x2A] = D_KEY_BACKSLASH;
        keys[0x2B] = D_KEY_COMMA;
        keys[0x18] = D_KEY_EQUAL;
        keys[0x32] = D_KEY_GRAVE_ACCENT;
        keys[0x21] = D_KEY_LEFT_BRACKET;
        keys[0x1B] = D_KEY_MINUS;
        keys[0x2F] = D_KEY_PERIOD;
        keys[0x1E] = D_KEY_RIGHT_BRACKET;
        keys[0x29] = D_KEY_SEMICOLON;
        keys[0x2C] = D_KEY_SLASH;
        keys[0x0A] = D_KEY_WORLD_1;

        keys[0x33] = D_KEY_BACKSPACE;
        keys[0x39] = D_KEY_CAPS_LOCK;
        keys[0x75] = D_KEY_DELETE;
        keys[0x7D] = D_KEY_DOWN;
        keys[0x77] = D_KEY_END;
        keys[0x24] = D_KEY_ENTER;
        keys[0x35] = D_KEY_ESCAPE;
        keys[0x73] = D_KEY_HOME;
        keys[0x72] = D_KEY_INSERT;
        keys[0x7B] = D_KEY_LEFT;
        keys[0x6E] = D_KEY_MENU;
        keys[0x47] = D_KEY_NUM_LOCK;
        keys[0x79] = D_KEY_PAGE_DOWN;
        keys[0x74] = D_KEY_PAGE_UP;
        keys[0x7C] = D_KEY_RIGHT;
        keys[0x31] = D_KEY_SPACE;
        keys[0x30] = D_KEY_TAB;
        keys[0x7E] = D_KEY_UP;

        keys[0x7A] = D_KEY_F1;   keys[0x78] = D_KEY_F2;   keys[0x63] = D_KEY_F3;
        keys[0x76] = D_KEY_F4;   keys[0x60] = D_KEY_F5;   keys[0x61] = D_KEY_F6;
        keys[0x62] = D_KEY_F7;   keys[0x64] = D_KEY_F8;   keys[0x65] = D_KEY_F9;
        keys[0x6D] = D_KEY_F10;  keys[0x67] = D_KEY_F11;  keys[0x6F] = D_KEY_F12;
        keys[0x69] = D_KEY_PRINT_SCREEN;
        keys[0x6B] = D_KEY_F14;  keys[0x71] = D_KEY_F15;  keys[0x6A] = D_KEY_F16;
        keys[0x40] = D_KEY_F17;  keys[0x4F] = D_KEY_F18;  keys[0x50] = D_KEY_F19;
        keys[0x5A] = D_KEY_F20;

        keys[0x3A] = D_KEY_LEFT_ALT;
        keys[0x3B] = D_KEY_LEFT_CONTROL;
        keys[0x38] = D_KEY_LEFT_SHIFT;
        keys[0x37] = D_KEY_LEFT_SUPER;
        keys[0x3D] = D_KEY_RIGHT_ALT;
        keys[0x3E] = D_KEY_RIGHT_CONTROL;
        keys[0x3C] = D_KEY_RIGHT_SHIFT;
        keys[0x36] = D_KEY_RIGHT_SUPER;

        keys[0x52] = D_KEY_KP_0;  keys[0x53] = D_KEY_KP_1;  keys[0x54] = D_KEY_KP_2;
        keys[0x55] = D_KEY_KP_3;  keys[0x56] = D_KEY_KP_4;  keys[0x57] = D_KEY_KP_5;
        keys[0x58] = D_KEY_KP_6;  keys[0x59] = D_KEY_KP_7;  keys[0x5B] = D_KEY_KP_8;
        keys[0x5C] = D_KEY_KP_9;
        keys[0x45] = D_KEY_KP_ADD;
        keys[0x41] = D_KEY_KP_DECIMAL;
        keys[0x4B] = D_KEY_KP_DIVIDE;
        keys[0x4C] = D_KEY_KP_ENTER;
        keys[0x51] = D_KEY_KP_EQUAL;
        keys[0x43] = D_KEY_KP_MULTIPLY;
        keys[0x4E] = D_KEY_KP_SUBTRACT;

        initialized = true;
        return keys;
    }

    inline int macKeyFromCode(unsigned short keyCode) {
        if (keyCode >= 256) return D_KEY_UNKNOWN;
        return macKeycodeTable()[keyCode];
    }

    inline int macKeyModifiers(NSUInteger flags) {
        int mods = 0;
        if (flags & NSEventModifierFlagShift)    mods |= D_MODIFIER_SHIFT;
        if (flags & NSEventModifierFlagControl)  mods |= D_MODIFIER_CONTROL;
        if (flags & NSEventModifierFlagOption)   mods |= D_MODIFIER_ALT;
        if (flags & NSEventModifierFlagCommand)  mods |= D_MODIFIER_SUPER;
        if (flags & NSEventModifierFlagCapsLock) mods |= D_MODIFIER_CAPS_LOCK;
        return mods;
    }

    // Modifier bit a modifier key press corresponds to, so flagsChanged: can
    // tell a press from a release (the event carries no up/down itself).
    inline NSUInteger macModFlagForKey(int key) {
        switch (key) {
            case D_KEY_LEFT_SHIFT:   case D_KEY_RIGHT_SHIFT:   return NSEventModifierFlagShift;
            case D_KEY_LEFT_CONTROL: case D_KEY_RIGHT_CONTROL: return NSEventModifierFlagControl;
            case D_KEY_LEFT_ALT:     case D_KEY_RIGHT_ALT:     return NSEventModifierFlagOption;
            case D_KEY_LEFT_SUPER:   case D_KEY_RIGHT_SUPER:   return NSEventModifierFlagCommand;
            case D_KEY_CAPS_LOCK:                              return NSEventModifierFlagCapsLock;
            default:                                           return 0;
        }
    }

}

#endif /* KeyCodesMac_h */
