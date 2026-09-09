// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef LinuxInputRouter_h
#define LinuxInputRouter_h

#include <X11/Xlib.h>

#include "Input.h"

namespace doriax {

    // X11 input-event translation. The editor and a game cannot share an event
    // loop -- the editor is its own ImGui platform backend, with viewports, IME,
    // selections and drag and drop on the same queue -- so a game's loop does
    // nothing but pass the input events here.
    class LinuxInputRouter {
    public:

        // Returns true when the event was consumed, false when the caller should
        // handle it itself.
        bool handleEvent(XEvent& event);

        // Keeps the tracked position in step when a game warps the pointer,
        // which reports no motion of its own while captured.
        void setMousePosition(double x, double y);

    private:
        void handleKey(XKeyEvent& event, bool pressed);

        // X11 carries no repeat flag, so a press for a key already held is one.
        // Indexed by X keycode, which Xlib bounds to 8..255.
        bool keyHeld[256]{};
        // The engine keeps seeing a continuous position while captured
        double mousePosX = 0.0;
        double mousePosY = 0.0;
    };

}

#endif /* LinuxInputRouter_h */
