// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// The macOS window, shared by the editor and by exported games: NSApplication
// setup, the NSWindow, cursor and MouseMode, fullscreen, sizing and the title.
// The drawable is the one thing that cannot be shared (MTKView for the editor
// and Metal games, NSOpenGLView for an OpenGL one), so it comes in through
// setContentView and this class never looks at its type.

#ifndef WindowMac_h
#define WindowMac_h

#include "System.h"

#include <functional>
#include <string>

namespace doriax {

    struct WindowMacConfig {
        std::string title = "Doriax";
        int width = 960;
        int height = 540;
        bool resizable = true;
    };

    class WindowMac {
    public:

        // Without a regular activation policy a non-bundled process launches as
        // a background agent: no dock icon, no menu bar, never active.
        static void setupApplication();

        // A menu bar with nothing but Quit, so Cmd+Q behaves as users expect.
        // The editor builds its own instead.
        static void installMinimalMenuBar(const std::string& applicationName);

        static void create(const WindowMacConfig& config);
        static void destroy();

        // NSWindow* and NSView*, for the drawable-specific code that owns them.
        static void* nativeWindow();
        static void* contentView();
        static void setContentView(void* view);
        static void setWindowDelegate(void* delegate);

        // Activates first: an inactive app's window cannot become key, so it
        // would render while every key press went elsewhere.
        static void show(bool focusContentView);

        // Applied after the window is on screen, so the zoomed and fullscreen
        // frames are the ones the window manager has already placed.
        static void applyInitialWindowMode(bool maximized, bool fullscreen);

        // Drawable size in backing pixels, which is what the engine works in
        static int getDrawableWidth();
        static int getDrawableHeight();
        // Recomputes from the content view and reports an actual change
        static void refreshDrawableSize();
        static void setDrawableResizeCallback(std::function<void(int, int)> callback);

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
        static void quit();

        static void setMouseCursor(CursorType type);
        static void setMouseMode(MouseMode mode);
        static void setMousePosition(float x, float y);

        // Cursor primitives. setMouseMode is these three composed the way a game
        // wants them; a host with its own cursor policy uses them directly.
        static void setCursorHidden(bool hidden);
        // Reapplies the invisible cursor after AppKit resets cursor rects, which
        // would undo the hide mid-drag. False when the cursor is visible.
        static bool applyHiddenCursorShape();
        // Detaches the pointer from the cursor, so deltas keep arriving past the
        // screen edge. Restoring puts it back where capture started.
        static void setCursorCaptured(bool captured, bool restorePosition);
        static bool isCursorCaptured();
        // macOS has no cursor confinement, so it is emulated by warping the
        // pointer back to the window bounds.
        static void confinePointerToWindow();
    };

}

#endif /* WindowMac_h */
