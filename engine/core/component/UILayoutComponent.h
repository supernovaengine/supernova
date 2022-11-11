#ifndef UI_LAYOUT_COMPONENT_H
#define UI_LAYOUT_COMPONENT_H

#include "util/FunctionSubscribe.h"

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
        FULL_SCREEN
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

        FunctionSubscribe<void()> onMouseMove;
        bool mouseMoved = false;

        bool focused = false;

        bool needUpdateSizes = false;
        bool needUpdateAnchors = false;
    };
    
}

#endif //UI_LAYOUT_COMPONENT_H