// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "App.h"
#include "renderer/Renderer.h"

#include <string>

namespace doriax::editor{

    struct EditorFrameState{
        bool& redrawRequested;
        double framePeriod = 1.0 / 60.0;
        bool forceRedraw = false;
        bool minimized = false;
        bool focused = false;
        int width = 0;
        int height = 0;
    };

    // Frame loop shared by the native backends, including the idle pacing
    class EditorFrame{
        private:
            Renderer* renderer = nullptr;
            App* app = nullptr;
            void (*platformNewFrame)() = nullptr;
            double lastActivityTime = 0.0;

            bool detectActivity(bool& redrawRequested);

        public:
            void init(Renderer& renderer, App& app, void (*platformNewFrame)());

            static std::string formatWindowTitle(const std::string& projectName);

            bool isIdle() const;

            // Returns false when the render target is lost and the editor must close.
            bool run(EditorFrameState& state);
    };

}
