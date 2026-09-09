// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "EditorFrame.h"

#include "AppSettings.h"
#include "Backend.h"

#include "imgui.h"

#include <chrono>
#include <thread>

using namespace doriax;
using namespace doriax::editor;

namespace {

// Seconds without activity before the loop starts idling
constexpr double IDLE_ENTER_DELAY = 0.5;

double monotonicSeconds(){
    using Clock = std::chrono::steady_clock;
    return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
}

}

void EditorFrame::init(Renderer& renderer, App& app, void (*platformNewFrame)()){
    this->renderer = &renderer;
    this->app = &app;
    this->platformNewFrame = platformNewFrame;
    lastActivityTime = monotonicSeconds();
}

std::string EditorFrame::formatWindowTitle(const std::string& projectName){
    return projectName.empty()
        ? "Empty project - Doriax Engine"
        : projectName + " - Doriax Engine";
}

bool EditorFrame::isIdle() const{
    return monotonicSeconds() - lastActivityTime > IDLE_ENTER_DELAY;
}

// Input, an active widget, a scene redraw or queued cross-thread work keeps the
// loop at full rate; its absence lets it idle next frame.
bool EditorFrame::detectActivity(bool& redrawRequested){
    ImGuiIO& io = ImGui::GetIO();

    // The code editor reads keys itself, with no io.WantTextInput or active item
    // to check. Keyboard range only: a resting stick would never let it idle.
    bool typing = io.InputQueueCharacters.Size > 0;
    for (int key = ImGuiKey_Keyboard_BEGIN; !typing && key < ImGuiKey_Keyboard_END; key++){
        typing = ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
    }

    // Consumed apart from the test below, which short-circuits
    const bool appRedraw = app->consumeRedrawRequest();

    const bool activity =
        io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f ||
        io.MouseWheel != 0.0f || io.MouseWheelH != 0.0f ||
        ImGui::IsAnyMouseDown() || typing || io.WantTextInput ||
        ImGui::IsAnyItemActive() || app->didRenderScene() ||
        app->hasPendingMainThreadTasks() || redrawRequested || appRedraw;
    redrawRequested = false;

    return activity;
}

bool EditorFrame::run(EditorFrameState& state){
    Project* project = app->getProject();
    const bool playSessionActive = project->isPlaySessionActive();

    // Hand the cursor back to the editor while a play session isn't actively
    // running (paused or loading) so a game-held cursor lock can't trap the mouse.
    Backend::setMouseControlSuspended(
        playSessionActive && !project->isMainScenePlaying());

    const double frameStart = monotonicSeconds();
    const bool idleFrame = !state.forceRedraw && isIdle();

    // Unfocused always drops synchronization, so a blocked present can never
    // stall the event loop that serves clipboard requests.
    const bool vsync = playSessionActive
        ? project->isVSyncEnabled()
        : AppSettings::getEditorVSyncEnabled();
    const bool frameSync = state.focused && vsync;
    if (!state.minimized && !renderer->updateTarget(state.width, state.height, frameSync)){
        return false;
    }

    // Consumed apart from the test below, which short-circuits
    const bool frameRequest = app->consumeFrameRequest();

    const bool renderRequested = state.forceRedraw || playSessionActive ||
                                 !idleFrame || state.redrawRequested ||
                                 app->hasPendingMainThreadTasks() || frameRequest;
    const bool frameReady = !state.minimized && renderRequested && renderer->beginFrame();

    renderer->newFrame();
    platformNewFrame();
    ImGui::NewFrame();

    if (frameReady){
        app->engineRender();
    }else{
        // engineRender() is what normally drains main-thread tasks. Keep draining
        // them so AI work, logging and async load callbacks still complete.
        app->processMainThreadTasks();
    }

    app->show();

    const bool activity = detectActivity(state.redrawRequested);
    if (activity){
        lastActivityTime = monotonicSeconds();
    }

    ImGui::Render();
    bool frameSubmitted = false;
    if (frameReady){
        frameSubmitted = renderer->endFrame(ImGui::GetDrawData(), state.width, state.height);
    }

    if (!frameReady || frameSubmitted){
        renderer->renderViewports(renderRequested || activity);
    }

    if (frameSubmitted){
        renderer->present();
    }
    renderer->purgeTextures();

    if (!state.focused || state.minimized){
        const int sleepMs = static_cast<int>(
            (state.framePeriod - (monotonicSeconds() - frameStart)) * 1000.0);
        if (sleepMs > 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        }
    }

    return true;
}
