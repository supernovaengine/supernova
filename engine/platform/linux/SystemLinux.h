// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// doriax::System over WindowLinux. It keeps no state of its own, so the editor
// driving WindowLinux directly and a game going through here stay in agreement.

#ifndef SystemLinux_h
#define SystemLinux_h

#include "System.h"

namespace doriax {

    class LinuxInputRouter;

    class SystemLinux: public System {
    public:

        // The router tracks the position the engine sees, which a warp has to
        // stay in step with; null when no router is installed.
        explicit SystemLinux(LinuxInputRouter* router = nullptr);

        int getScreenWidth() override;
        int getScreenHeight() override;

        int getSampleCount() override;

#if defined(SOKOL_VULKAN)
        // The GL backend renders into the default framebuffer, which the base
        // class already describes. Vulkan has to hand sokol the device and the
        // image acquired for this frame instead.
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
        LinuxInputRouter* router;
    };

}

#endif /* SystemLinux_h */
