// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "texture/Texture.h"
#include "imgui.h"

namespace doriax::editor {

    class AboutWindow {
    private:
        bool m_isOpen = false;
        bool m_logoLoaded = false;
        Texture m_logo;

        void drawAbout();

    public:
        AboutWindow() = default;
        ~AboutWindow() = default;

        void open();
        void show();
        bool isOpen() const { return m_isOpen; }
    };

}
