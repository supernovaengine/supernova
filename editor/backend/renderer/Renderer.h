// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "System.h"

#include "imgui.h"

#include <memory>

namespace doriax{
    class TextureRender;
}

namespace doriax::editor{

    // Window-system calls the renderer needs, filled in by the backend
    struct RendererPlatform{
        #if defined(SOKOL_VULKAN)
        const char* surfaceExtension = nullptr;
        int (*createSurface)(ImGuiViewport* viewport, ImU64 instance,
                             const void* allocator, ImU64* surface) = nullptr;
        void (*requestRedraw)() = nullptr;
        #else
        bool (*createContext)() = nullptr;
        void (*destroyContext)() = nullptr;
        void (*makeCurrent)(ImGuiViewport* viewport) = nullptr;
        void (*swapBuffers)(ImGuiViewport* viewport) = nullptr;
        void (*setSwapInterval)(int interval) = nullptr;
        #endif
    };

    // Graphics-API half of the native backends: device or context, the ImGui
    // renderer binding and presentation
    class Renderer{
        private:
            struct State;
            std::unique_ptr<State> state;

        public:
            Renderer();
            ~Renderer();

            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            bool init(const RendererPlatform& platform, int width, int height, bool synchronized);
            void shutdownImGui();

            // Resizes the render target and applies the frame-sync mode
            bool updateTarget(int width, int height, bool synchronized);
            bool beginFrame();
            void newFrame();
            bool endFrame(ImDrawData* drawData, int width, int height);
            void present();
            void renderViewports(bool render);

            ImTextureID getTexture(TextureRender* texture);
            // Releases ImGui bindings of textures the engine has destroyed
            void purgeTextures();

            #if defined(SOKOL_VULKAN)
            sg_environment getSokolEnvironment() const;
            sg_swapchain getSokolSwapchain() const;
            #endif
    };

}
