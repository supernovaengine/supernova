// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef GamepadWin_h
#define GamepadWin_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <xinput.h>

#include <array>

namespace doriax {

    // XInput polling shared by the editor backend and the exported game, so a
    // controller behaves the same in Play mode and in the shipped build.
    // Emits Engine::systemGamepad* only on change.
    class GamepadWin {
    public:
        static constexpr int COUNT = XUSER_MAX_COUNT;
        static constexpr int BUTTON_COUNT = 15;
        static constexpr int AXIS_COUNT = 6;

        // Loads the newest available XInput DLL. Polling is a no-op if none is.
        void init();
        // Disconnects every pad (so the engine sees the events) and unloads.
        void shutdown();
        void poll();

    private:
        struct Pad {
            bool connected = false;
            std::array<unsigned char, BUTTON_COUNT> buttons{};
            std::array<float, AXIS_COUNT> axes{};
        };

        using GetStateProc = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);

        void disconnect(int id);

        HMODULE library = nullptr;
        GetStateProc getState = nullptr;
        ULONGLONG nextScan = 0;
        std::array<Pad, COUNT> pads;
    };

}

#endif /* GamepadWin_h */
