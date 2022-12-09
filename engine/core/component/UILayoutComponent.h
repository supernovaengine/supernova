#ifndef UI_LAYOUT_COMPONENT_H
#define UI_LAYOUT_COMPONENT_H

#include "util/FunctionSubscribe.h"
#include "math/Rect.h"

namespace Supernova{

    enum class AnchorPreset{
        NONE,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        CENTER_LEFT,
        CENTER_TOP,
        CENTER_RIGHT,
        CENTER_BOTTOM,
        CENTER,
        LEFT_WIDE,
        TOP_WIDE,
        RIGHT_WIDE,
        BOTTOM_WIDE,
        VERTICAL_CENTER_WIDE,
        HORIZONTAL_CENTER_WIDE,
        FULL_LAYOUT
    };

    struct UILayoutComponent{
        // UI part
        int width = 0;
        int height = 0;

        float anchorLeft = 0;
        float anchorTop = 0;
        float anchorRight = 0;
        float anchorBottom = 0;

        int marginLeft = 0;
        int marginTop = 0;
        int marginRight = 0;
        int marginBottom = 0;

        AnchorPreset anchorPreset = AnchorPreset::NONE;
        bool usingAnchors = false;

        int containerBoxIndex = -1;

        Rect scissor = Rect(0, 0, 0, 0);
        bool ignoreScissor = false; // ignore parent scissor

        bool needUpdateSizes = false;
    };
    
}

#endif //UI_LAYOUT_COMPONENT_H