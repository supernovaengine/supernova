// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// The X11 window, shared by the editor and by exported games: the display
// connection, the window, the _NET_WM_STATE round trips behind fullscreen and
// maximize, sizing, the title and the cursor -- including the XGrabPointer
// semantics CAPTURED and CONFINED are built from.
//
// The editor is its own ImGui platform backend, so it already owns a display
// and a window and hands them over with adopt() instead of calling create().

#ifndef WindowLinux_h
#define WindowLinux_h

#include <X11/Xlib.h>

#include "System.h"

#include <functional>
#include <string>

namespace doriax {

    struct WindowLinuxConfig {
        std::string title = "Doriax";
        // WM_CLASS. Desktop environments match it against an installed
        // <app_id>.desktop entry, which is where the window icon comes from.
        std::string appId = "Doriax";
        int width = 960;
        int height = 540;
        bool resizable = true;
        // The visual the drawable needs; a GLX game passes the one that matches
        // its framebuffer configuration.
        Visual* visual = nullptr;
        int depth = 0;
    };

    class WindowLinux {
    public:

        // Opens the X connection and interns the atoms the window state needs.
        static bool openDisplay();
        static bool create(const WindowLinuxConfig& config);
        // A host that already owns an X connection and window hands them over
        // instead of calling openDisplay/create, and keeps ownership of both.
        static void adopt(Display* display, int screen, Window window);
        static void destroy();

        static Display* display();
        static int screen();
        static Window handle();

        static void show();

        static int getWidth();
        static int getHeight();
        // Called with a ConfigureNotify size; reports an actual change
        static void updateSize(int width, int height);
        static void setResizeCallback(std::function<void(int, int)> callback);

        static bool isFullscreen();
        static void requestFullscreen();
        static void exitFullscreen();

        static bool isMaximized();
        static void maximize();
        static void restore();

        static void setSize(int width, int height);
        static bool isResizable();
        static void setResizable(bool resizable);

        static void setTitle(const std::string& title);

        // Sends WM_DELETE_WINDOW to our own window, so the caller's event loop
        // runs its shutdown instead of the window vanishing inside a script call.
        static void quit();
        // True for that message and for the window manager's own close request
        static bool isCloseEvent(const XEvent& event);

        static Cursor invisibleCursor();
        static void setMouseCursor(CursorType type);
        static void setMouseMode(MouseMode mode);
        static void setMousePosition(float x, float y);

        // One grab covers both modes: `confined` keeps the pointer inside the
        // window, `relative` is the mouse-look that reads motion from the warp.
        static void setPointer(bool relative, bool confined, Cursor cursor);
        static bool isRelativeMouse();
        static bool isPointerGrabbed();
        // Drops the grab but keeps the relative mode, so a window that has lost
        // focus cannot trap the pointer and can re-grab when it comes back.
        static void releasePointerGrab();
        // Drops the grab and the relative mode both, without touching the
        // cursor shape the host has applied.
        static void releasePointer();
        // Keeps the pointer where it is instead of warping it back, for a window
        // that has lost focus and no longer owns the cursor.
        static void discardSavedPointer();
        // X11 has no relative motion without XInput2, so CAPTURED warps the
        // pointer back to the centre and measures the next event against it.
        static void centerPointer();
        static int lastWarpX();
        static int lastWarpY();
    };

}

#endif /* WindowLinux_h */
