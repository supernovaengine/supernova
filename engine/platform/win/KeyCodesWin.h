// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// Win32 -> engine key translation. The engine's key constants are GLFW's
// (see core/Input.h), so this reproduces GLFW's Win32 scancode table rather
// than mapping virtual-key codes: scancodes are layout independent, which is
// what makes W/A/S/D land on the same keys under AZERTY as under QWERTY, and
// it is the only way to tell the two Shift/Control/Alt keys and the keypad
// apart. Keeping the mapping identical to GLFW is what lets a project behave
// the same here as it did on the old GLFW and sokol_app backends.

#ifndef KeyCodesWin_h
#define KeyCodesWin_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "Input.h"

namespace doriax {

    // Indexed by scancode including the extended bit (0x100), so 0..0x1ff.
    inline const int* winScancodeTable() {
        static int keys[0x200];
        static bool initialized = false;
        if (initialized) return keys;

        for (int i = 0; i < 0x200; ++i) keys[i] = D_KEY_UNKNOWN;

        keys[0x00B] = D_KEY_0;
        keys[0x002] = D_KEY_1;
        keys[0x003] = D_KEY_2;
        keys[0x004] = D_KEY_3;
        keys[0x005] = D_KEY_4;
        keys[0x006] = D_KEY_5;
        keys[0x007] = D_KEY_6;
        keys[0x008] = D_KEY_7;
        keys[0x009] = D_KEY_8;
        keys[0x00A] = D_KEY_9;
        keys[0x01E] = D_KEY_A;
        keys[0x030] = D_KEY_B;
        keys[0x02E] = D_KEY_C;
        keys[0x020] = D_KEY_D;
        keys[0x012] = D_KEY_E;
        keys[0x021] = D_KEY_F;
        keys[0x022] = D_KEY_G;
        keys[0x023] = D_KEY_H;
        keys[0x017] = D_KEY_I;
        keys[0x024] = D_KEY_J;
        keys[0x025] = D_KEY_K;
        keys[0x026] = D_KEY_L;
        keys[0x032] = D_KEY_M;
        keys[0x031] = D_KEY_N;
        keys[0x018] = D_KEY_O;
        keys[0x019] = D_KEY_P;
        keys[0x010] = D_KEY_Q;
        keys[0x013] = D_KEY_R;
        keys[0x01F] = D_KEY_S;
        keys[0x014] = D_KEY_T;
        keys[0x016] = D_KEY_U;
        keys[0x02F] = D_KEY_V;
        keys[0x011] = D_KEY_W;
        keys[0x02D] = D_KEY_X;
        keys[0x015] = D_KEY_Y;
        keys[0x02C] = D_KEY_Z;

        keys[0x028] = D_KEY_APOSTROPHE;
        keys[0x02B] = D_KEY_BACKSLASH;
        keys[0x033] = D_KEY_COMMA;
        keys[0x00D] = D_KEY_EQUAL;
        keys[0x029] = D_KEY_GRAVE_ACCENT;
        keys[0x01A] = D_KEY_LEFT_BRACKET;
        keys[0x00C] = D_KEY_MINUS;
        keys[0x034] = D_KEY_PERIOD;
        keys[0x01B] = D_KEY_RIGHT_BRACKET;
        keys[0x027] = D_KEY_SEMICOLON;
        keys[0x035] = D_KEY_SLASH;
        keys[0x056] = D_KEY_WORLD_2;

        keys[0x00E] = D_KEY_BACKSPACE;
        keys[0x153] = D_KEY_DELETE;
        keys[0x14F] = D_KEY_END;
        keys[0x01C] = D_KEY_ENTER;
        keys[0x001] = D_KEY_ESCAPE;
        keys[0x147] = D_KEY_HOME;
        keys[0x152] = D_KEY_INSERT;
        keys[0x15D] = D_KEY_MENU;
        keys[0x151] = D_KEY_PAGE_DOWN;
        keys[0x149] = D_KEY_PAGE_UP;
        keys[0x045] = D_KEY_PAUSE;
        keys[0x039] = D_KEY_SPACE;
        keys[0x00F] = D_KEY_TAB;
        keys[0x03A] = D_KEY_CAPS_LOCK;
        keys[0x145] = D_KEY_NUM_LOCK;
        keys[0x046] = D_KEY_SCROLL_LOCK;

        keys[0x03B] = D_KEY_F1;
        keys[0x03C] = D_KEY_F2;
        keys[0x03D] = D_KEY_F3;
        keys[0x03E] = D_KEY_F4;
        keys[0x03F] = D_KEY_F5;
        keys[0x040] = D_KEY_F6;
        keys[0x041] = D_KEY_F7;
        keys[0x042] = D_KEY_F8;
        keys[0x043] = D_KEY_F9;
        keys[0x044] = D_KEY_F10;
        keys[0x057] = D_KEY_F11;
        keys[0x058] = D_KEY_F12;
        keys[0x064] = D_KEY_F13;
        keys[0x065] = D_KEY_F14;
        keys[0x066] = D_KEY_F15;
        keys[0x067] = D_KEY_F16;
        keys[0x068] = D_KEY_F17;
        keys[0x069] = D_KEY_F18;
        keys[0x06A] = D_KEY_F19;
        keys[0x06B] = D_KEY_F20;
        keys[0x06C] = D_KEY_F21;
        keys[0x06D] = D_KEY_F22;
        keys[0x06E] = D_KEY_F23;
        keys[0x076] = D_KEY_F24;

        keys[0x038] = D_KEY_LEFT_ALT;
        keys[0x01D] = D_KEY_LEFT_CONTROL;
        keys[0x02A] = D_KEY_LEFT_SHIFT;
        keys[0x15B] = D_KEY_LEFT_SUPER;
        keys[0x137] = D_KEY_PRINT_SCREEN;
        keys[0x138] = D_KEY_RIGHT_ALT;
        keys[0x11D] = D_KEY_RIGHT_CONTROL;
        keys[0x036] = D_KEY_RIGHT_SHIFT;
        keys[0x15C] = D_KEY_RIGHT_SUPER;

        keys[0x150] = D_KEY_DOWN;
        keys[0x14B] = D_KEY_LEFT;
        keys[0x14D] = D_KEY_RIGHT;
        keys[0x148] = D_KEY_UP;

        keys[0x052] = D_KEY_KP_0;
        keys[0x04F] = D_KEY_KP_1;
        keys[0x050] = D_KEY_KP_2;
        keys[0x051] = D_KEY_KP_3;
        keys[0x04B] = D_KEY_KP_4;
        keys[0x04C] = D_KEY_KP_5;
        keys[0x04D] = D_KEY_KP_6;
        keys[0x047] = D_KEY_KP_7;
        keys[0x048] = D_KEY_KP_8;
        keys[0x049] = D_KEY_KP_9;
        keys[0x04E] = D_KEY_KP_ADD;
        keys[0x053] = D_KEY_KP_DECIMAL;
        keys[0x135] = D_KEY_KP_DIVIDE;
        keys[0x11C] = D_KEY_KP_ENTER;
        keys[0x059] = D_KEY_KP_EQUAL;
        keys[0x037] = D_KEY_KP_MULTIPLY;
        keys[0x04A] = D_KEY_KP_SUBTRACT;

        initialized = true;
        return keys;
    }

    // Translates a WM_KEYDOWN/WM_KEYUP pair into an engine key constant.
    inline int winKeyFromMessage(WPARAM virtualKey, LPARAM lParam) {
        if (virtualKey == VK_PROCESSKEY) {
            // Consumed by an IME; the resulting text arrives as WM_CHAR.
            return D_KEY_UNKNOWN;
        }

        int scancode = static_cast<int>(HIWORD(lParam) & (KF_EXTENDED | 0xff));
        if (!scancode) {
            // Synthetic events (an on-screen keyboard, SendInput) carry no
            // scancode, so recover one from the virtual key.
            scancode = static_cast<int>(MapVirtualKeyW(
                static_cast<UINT>(virtualKey), MAPVK_VK_TO_VSC));
        }

        // Alt+PrintScreen reports a different scancode than PrintScreen alone.
        if (scancode == 0x54) scancode = 0x137;
        // Ctrl+Pause reports Break; the engine only knows Pause.
        if (scancode == 0x146) scancode = 0x45;
        // Some keyboards send the numpad Enter without the extended bit.
        if (scancode == 0x136) scancode = 0x36;

        if (scancode < 0 || scancode >= 0x200) return D_KEY_UNKNOWN;
        return winScancodeTable()[scancode];
    }

    // GLFW-compatible modifier bits for the current keyboard state.
    inline int winKeyModifiers() {
        int mods = 0;
        if (GetKeyState(VK_SHIFT) & 0x8000) mods |= D_MODIFIER_SHIFT;
        if (GetKeyState(VK_CONTROL) & 0x8000) mods |= D_MODIFIER_CONTROL;
        if (GetKeyState(VK_MENU) & 0x8000) mods |= D_MODIFIER_ALT;
        if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
            mods |= D_MODIFIER_SUPER;
        if (GetKeyState(VK_CAPITAL) & 1) mods |= D_MODIFIER_CAPS_LOCK;
        if (GetKeyState(VK_NUMLOCK) & 1) mods |= D_MODIFIER_NUM_LOCK;
        return mods;
    }

}

#endif /* KeyCodesWin_h */
