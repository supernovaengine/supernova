// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT
// Vulkan device and swapchain for the native desktop backends. The window system
// is the only part that differs per platform, so the backend passes in the
// surface extension and a surface constructor and shares everything else.

#ifndef VulkanContext_h
#define VulkanContext_h

#include "System.h"

#include <vulkan/vulkan.h>

namespace doriax {

    struct VulkanContextConfig {
        // VK_KHR_win32_surface, VK_KHR_xlib_surface, VK_EXT_metal_surface, ...
        const char* surfaceExtension = nullptr;
        // Called once, before the physical device is picked: presentation
        // support is part of choosing one.
        VkResult (*createSurface)(VkInstance instance, VkSurfaceKHR* surface) = nullptr;
        bool vsync = true;
    };

    // One window and one device per process, so the state is static like
    // WindowWin's and the System implementation can reach it directly.
    class VulkanContext {
    public:

        // Must run before Engine::systemViewLoaded(), where sokol reads the environment
        static bool create(const VulkanContextConfig& config);
        static void destroy();

        // Acquires the image the frame renders into, rebuilding the swapchain
        // when the surface changed size. Frames with nothing to draw into (a
        // minimized window) leave the swapchain invalid and sokol skips the pass.
        static void beginFrame();
        static void endFrame();

        static sg_environment environment();
        static sg_swapchain swapchain();
    };

}

#endif /* VulkanContext_h */
