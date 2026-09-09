// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "imgui.h"

namespace doriax::editor {

    class Theme {
    public:
        struct Colors {
            static ImVec4 ButtonActivated;
            static ImVec4 FileCardBackground;
            static ImVec4 FileCardBackgroundHovered;
            static ImVec4 SubtleText;
            static ImVec4 filenameLabel;
            static ImVec4 ExtEntityButton;
            static ImVec4 ExtEntityButtonHovered;
            static ImVec4 ExtEntityButtonActive;
            static ImVec4 NestedHeader;
            static ImVec4 NestedHeaderHovered;
            static ImVec4 NestedHeaderActive;
            static ImVec4 DisabledGreenText;
            static ImVec4 ChildSceneText;
            // Marks the viewport sub-selection (tiles, instances, occluder points)
            // in Properties lists; same orange as the viewport selection outline.
            static ImVec4 SubSelectionText;
            // Amber warning text, e.g. alert notes; matches OutputWindow's Warning log color.
            static ImVec4 WarningText;
        };

        static void apply();
        static void applyDpiScale(float dpiScale);

        static float dpiScale();
        static float dpi(float value);
        static ImVec2 dpi(const ImVec2& value);
    };

}

