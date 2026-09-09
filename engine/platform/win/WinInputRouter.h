// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef WinInputRouter_h
#define WinInputRouter_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "Input.h"

namespace doriax {

    // Win32 input-message translation. The editor and a game cannot share a
    // WNDPROC -- the editor's hands messages to ImGui and keeps a virtual
    // pointer of its own -- so a game's does nothing but pass them here.
    class WinInputRouter {
    public:

        // Returns true when the message was fully handled and the caller should
        // return `result`; false when it should carry on to DefWindowProc, which
        // key messages need so WM_CHAR is still generated and Alt+F4 works.
        bool handleMessage(HWND window, UINT message, WPARAM wParam,
                           LPARAM lParam, LRESULT& result);

        // Keeps the tracked position in step when a game warps the pointer,
        // which reports no motion of its own while captured.
        void setMousePosition(double x, double y);

    private:
        // The engine keeps seeing a continuous position while captured
        double mousePosX = 0.0;
        double mousePosY = 0.0;
    };

}

#endif /* WinInputRouter_h */
