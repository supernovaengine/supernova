// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "LinuxInputRouter.h"

#include "KeyCodesLinux.h"
#include "WindowLinux.h"

#include "Engine.h"

// XLookupKeysym and XLookupString are declared here, not in Xlib.h
#include <X11/Xutil.h>

using namespace doriax;

void LinuxInputRouter::setMousePosition(double x, double y) {
    mousePosX = x;
    mousePosY = y;
}

void LinuxInputRouter::handleKey(XKeyEvent& event, bool pressed) {
    const unsigned int keycode = event.keycode & 0xff;

    // Level-0 keysym keeps a key on its printed position, and where a layout
    // spells none the position it sits in answers for it
    const KeySym symbol = XLookupKeysym(&event, 0);
    int key = linuxKeyFromSym(symbol);
    if (key == D_KEY_UNKNOWN) key = linuxKeycodeTable(event.display)[keycode];
    const int mods = linuxKeyModifiers(event.state);

    // handleEvent swallows the release half of a repeat pair, so a press
    // arriving while the key is still held is a repeat.
    const bool repeat = pressed && keyHeld[keycode];
    keyHeld[keycode] = pressed;

    if (key != D_KEY_UNKNOWN) {
        if (pressed) Engine::systemKeyDown(key, repeat, mods);
        else Engine::systemKeyUp(key, false, mods);
    }

    if (!pressed) return;

    // XLookupString returns ISO Latin-1, so each byte is already the codepoint.
    // That holds because a game never calls setlocale, leaving LC_CTYPE at "C";
    // non-Latin-1 text would need an XIM/XIC and Xutf8LookupString, the way the
    // editor backend does it, plus a UTF-8 decode here.
    char text[32];
    KeySym ignored = 0;
    const int length = XLookupString(&event, text, sizeof(text), &ignored, nullptr);
    for (int i = 0; i < length; ++i) {
        const unsigned char codepoint = static_cast<unsigned char>(text[i]);
        if (codepoint >= 32 || codepoint == '\t' || codepoint == '\b' ||
            codepoint == '\r' || codepoint == 0x1b)
            Engine::systemCharInput(codepoint);
    }
}

bool LinuxInputRouter::handleEvent(XEvent& event) {
    Display* display = WindowLinux::display();

    switch (event.type) {
        case KeyPress:
            handleKey(event.xkey, true);
            return true;

        case KeyRelease: {
            // X11 sends a press/release pair for auto-repeat; drop the release
            // half so the engine sees a held key.
            if (display && XPending(display)) {
                XEvent next{};
                XPeekEvent(display, &next);
                if (next.type == KeyPress &&
                    next.xkey.keycode == event.xkey.keycode &&
                    next.xkey.time - event.xkey.time < 20)
                    return true;
            }
            handleKey(event.xkey, false);
            return true;
        }

        case MotionNotify: {
            const int mods = linuxKeyModifiers(event.xmotion.state);
            if (WindowLinux::isRelativeMouse()) {
                const int dx = event.xmotion.x - WindowLinux::lastWarpX();
                const int dy = event.xmotion.y - WindowLinux::lastWarpY();
                if (dx == 0 && dy == 0) return true; // our own warp
                mousePosX += dx;
                mousePosY += dy;
                WindowLinux::centerPointer();
            } else {
                mousePosX = event.xmotion.x;
                mousePosY = event.xmotion.y;
            }
            Engine::systemMouseMove(float(mousePosX), float(mousePosY), mods);
            return true;
        }

        case ButtonPress:
        case ButtonRelease: {
            const bool pressed = event.type == ButtonPress;
            const int mods = linuxKeyModifiers(event.xbutton.state);
            const unsigned int button = event.xbutton.button;

            // X11 reports the wheel as buttons 4-7
            if (button >= Button4 && button <= 7) {
                if (!pressed) return true;
                const float x = (button == 6) ? -1.0f : (button == 7) ? 1.0f : 0.0f;
                const float y = (button == Button4) ? 1.0f
                              : (button == Button5) ? -1.0f : 0.0f;
                Engine::systemMouseScroll(x, y, mods);
                return true;
            }

            if (!WindowLinux::isRelativeMouse()) {
                mousePosX = event.xbutton.x;
                mousePosY = event.xbutton.y;
            }
            int engineButton = D_MOUSE_BUTTON_LEFT;
            if (button == Button2) engineButton = D_MOUSE_BUTTON_MIDDLE;
            else if (button == Button3) engineButton = D_MOUSE_BUTTON_RIGHT;
            else if (button > 7) engineButton = int(button) - 8 + D_MOUSE_BUTTON_4;

            if (pressed)
                Engine::systemMouseDown(engineButton, float(mousePosX),
                                        float(mousePosY), mods);
            else
                Engine::systemMouseUp(engineButton, float(mousePosX),
                                      float(mousePosY), mods);
            return true;
        }

        case EnterNotify:
            Engine::systemMouseEnter();
            return true;

        case LeaveNotify:
            Engine::systemMouseLeave();
            return true;

        default:
            return false;
    }
}
