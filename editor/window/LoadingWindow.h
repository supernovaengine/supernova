// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "thread/ResourceProgress.h"

namespace doriax::editor {

    class LoadingWindow {
    private:
        bool wasShowing = false;

    public:
        LoadingWindow();
        ~LoadingWindow();

        void show();

    private:
        void drawProgressModal(const OverallBuildProgress& progress);
    };

}