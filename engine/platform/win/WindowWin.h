// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// The Win32 window, shared by the editor and by exported games: the window
// class, the window, cursor and MouseMode, the cursor clip, fullscreen, sizing
// and the title. The caller supplies its own WNDPROC, which is the one thing
// the two cannot share -- the editor's goes to ImGui and a game's to
// WinInputRouter.

#ifndef WindowWin_h
#define WindowWin_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "System.h"

#include <functional>
#include <string>

namespace doriax {

    // UTF-8 to UTF-16 for the wide Win32 entry points. Falls back to the active
    // code page when the text is not valid UTF-8.
    std::wstring winUtf8ToWide(const std::string& text);

    struct WindowWinConfig {
        std::string title = "Doriax";
        int width = 960;
        int height = 540;
        bool resizable = true;
        bool maximized = false;

        // The caller's message handler, and the class it is registered under.
        WNDPROC windowProc = nullptr;
        const wchar_t* className = L"DoriaxWindow";
        // OpenGL keeps a device context per window, which needs CS_OWNDC;
        // a Vulkan surface does not.
        UINT classStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        HICON icon = nullptr;
        // The editor hosts child windows; a game's client area is one drawable
        bool clipChildren = false;
        // Trims the size to the primary monitor's work area, for callers
        // restoring one saved on another monitor or at another display scaling
        bool clampToWorkArea = false;
    };

    class WindowWin {
    public:

        // Declares the process per-monitor DPI aware. create() calls it; a
        // caller reading monitor metrics before that needs it earlier.
        static void enableDpiAwareness();

        // Registers the class, creates the window and registers the raw mouse
        // device that CAPTURED mode reads. Does not show the window.
        static bool create(const WindowWinConfig& config);
        static void destroy();

        static HWND handle();
        static HINSTANCE instance();

        static void show(bool maximized);

        // Process wide, not window specific: the editor's detached viewports are
        // separate top-level windows.
        static bool hasFocus();

        // Desktop scaling of the monitor a window is on, or of the primary one
        // when there is no window yet. 1.0 at 96 DPI.
        static float monitorScale(HWND window);

        // Client area in pixels, which is what the engine works in
        static void getClientSize(int& width, int& height);
        static int getClientWidth();
        static int getClientHeight();
        // Recomputes from the window and reports an actual change
        static void refreshClientSize();
        static void setClientSizeCallback(std::function<void(int, int)> callback);

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
        // Posts WM_CLOSE, so the caller's message handler runs its own shutdown
        // instead of the window disappearing from under a script call.
        static void quit();

        static void setMouseCursor(CursorType type);
        static void setMouseMode(MouseMode mode);
        // Client-area coordinates, matching what the engine reports
        static void setMousePosition(float x, float y);

        // Cursor primitives. setMouseMode is these composed the way a game wants
        // them; a host with its own cursor policy uses them directly.
        static void setCursorHidden(bool hidden);
        static bool isCursorHidden();
        // CAPTURED reads motion from WM_INPUT instead of the cursor position,
        // which stops at the screen edge.
        static void setRelativeMouse(bool enabled, bool restorePosition);
        static bool isRelativeMouse();
        // CONFINED: the cursor stays visible but cannot leave the client area
        static void setCursorConfined(bool confined);
        // Re-applies the clip for the current flags and focus state. Call from
        // WM_SIZE / WM_MOVE / WM_SETFOCUS and once per frame.
        static void updateCursorClip();
        static void releaseCursorClip();
        // Releases the capture so a background game cannot trap the pointer,
        // and forgets the saved position along with it.
        static void handleFocusLost();

        // Drops the baseline used to convert absolute raw input to motion.
        static void resetRawMouseDelta();
        // Reads motion from one WM_INPUT message, including absolute devices.
        // Returns false when the message does not produce a usable delta.
        static bool readRawMouseDelta(HRAWINPUT handle, LONG& deltaX, LONG& deltaY);
    };

}

#endif /* WindowWin_h */
