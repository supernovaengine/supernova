// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// doriax::System over WindowWin. It keeps no state of its own, so the editor
// driving WindowWin directly and a game going through here stay in agreement.

#ifndef SystemWin_h
#define SystemWin_h

#include "System.h"

namespace doriax {

    class WinInputRouter;

    class SystemWin: public System {
    public:

        // The router tracks the position the engine sees, which a warp has to
        // stay in step with; null when no router is installed.
        explicit SystemWin(WinInputRouter* router = nullptr);

        int getScreenWidth() override;
        int getScreenHeight() override;

        int getSampleCount() override;

#if defined(SOKOL_VULKAN) || defined(SOKOL_D3D11)
        // The GL backend renders into the default framebuffer, which the base
        // class already describes. Vulkan and D3D11 have to hand sokol the
        // device and the views this frame draws into instead.
        sg_environment getSokolEnvironment() override;
        sg_swapchain getSokolSwapchain() override;
#endif

        bool isFullscreen() override;
        void requestFullscreen() override;
        void exitFullscreen() override;

        bool isWindowMaximized() override;
        void maximizeWindow() override;
        void restoreWindow() override;
        void setWindowSize(int width, int height) override;
        bool isWindowResizable() override;
        void setWindowResizable(bool resizable) override;
        void setWindowTitle(const std::string& title) override;
        void quit() override;

        void setMouseCursor(CursorType type) override;
        void setMouseMode(MouseMode mode) override;
        void setMousePosition(float x, float y) override;

        std::string getAssetPath() override;
        std::string getUserDataPath() override;
        std::string getLuaPath() override;
        std::string getShaderPath() override;

    private:
        WinInputRouter* router;
    };

}

#endif /* SystemWin_h */
