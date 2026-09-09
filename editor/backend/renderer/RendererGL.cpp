// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Renderer.h"

#include "CameraRender.h"
#include "TextureRender.h"

#include "imgui_impl_opengl3.h"

using namespace doriax;
using namespace doriax::editor;

struct Renderer::State{
    RendererPlatform platform;
    CameraRender render;
    int swapInterval = -1;

    static Renderer* of(ImGuiViewport* viewport){
        return static_cast<Renderer*>(viewport->RendererUserData);
    }

    static void renderWindow(ImGuiViewport* viewport, void*){
        of(viewport)->state->platform.makeCurrent(viewport);
    }

    static void swapBuffers(ImGuiViewport* viewport, void*){
        of(viewport)->state->platform.swapBuffers(viewport);
    }
};

Renderer::Renderer() : state(std::make_unique<State>()){}

Renderer::~Renderer(){
    if (state->platform.destroyContext){
        state->platform.destroyContext();
    }
}

bool Renderer::init(const RendererPlatform& platform, int, int, bool synchronized){
    state->platform = platform;
    if (!state->platform.createContext()){
        return false;
    }
    updateTarget(0, 0, synchronized);
    if (!ImGui_ImplOpenGL3_Init("#version 410")){
        return false;
    }

    // ImGui_ImplOpenGL3 draws the viewports, but the context switch and the
    // presentation are up to the platform
    ImGuiPlatformIO& platformIo = ImGui::GetPlatformIO();
    ImGui::GetMainViewport()->RendererUserData = this;
    platformIo.Renderer_CreateWindow = [](ImGuiViewport* viewport){
        viewport->RendererUserData = ImGui::GetMainViewport()->RendererUserData;
    };
    platformIo.Renderer_DestroyWindow = [](ImGuiViewport* viewport){
        viewport->RendererUserData = nullptr;
    };
    platformIo.Platform_RenderWindow = &State::renderWindow;
    platformIo.Platform_SwapBuffers = &State::swapBuffers;

    return true;
}

void Renderer::shutdownImGui(){
    ImGui_ImplOpenGL3_Shutdown();

    ImGuiPlatformIO& platformIo = ImGui::GetPlatformIO();
    platformIo.Platform_RenderWindow = nullptr;
    platformIo.Platform_SwapBuffers = nullptr;
    ImGui::GetMainViewport()->RendererUserData = nullptr;
}

bool Renderer::updateTarget(int, int, bool synchronized){
    const int interval = synchronized ? 1 : 0;
    if (interval != state->swapInterval){
        state->platform.setSwapInterval(interval);
        state->swapInterval = interval;
    }
    return true;
}

bool Renderer::beginFrame(){
    state->platform.makeCurrent(nullptr);
    return true;
}

void Renderer::newFrame(){
    ImGui_ImplOpenGL3_NewFrame();
}

bool Renderer::endFrame(ImDrawData* drawData, int width, int height){
    state->render.setClearColor(Vector4(0.45f, 0.55f, 0.60f, 1.00f));
    state->render.startRenderPass(width, height);
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
    state->render.endRenderPass();

    return true;
}

void Renderer::present(){
    state->platform.swapBuffers(nullptr);
}

void Renderer::renderViewports(bool render){
    if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)){
        return;
    }
    ImGui::UpdatePlatformWindows();
    if (render){
        ImGui::RenderPlatformWindowsDefault();
        // Viewports swap unsynchronized, and some window systems keep the
        // interval with the context instead of the drawable.
        state->platform.makeCurrent(nullptr);
        state->platform.setSwapInterval(state->swapInterval);
    }
}

ImTextureID Renderer::getTexture(TextureRender* texture){
    return static_cast<ImTextureID>(texture->getGLHandler());
}

// Textures are bound by GL name, so nothing is cached here
void Renderer::purgeTextures(){
}
