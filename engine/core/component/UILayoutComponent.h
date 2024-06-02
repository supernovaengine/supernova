//
// (c) 2024 Eduardo Doria.
//

#ifndef UI_LAYOUT_COMPONENT_H
#define UI_LAYOUT_COMPONENT_H

#include "util/FunctionSubscribe.h"
#include "math/Rect.h"
#include "math/Quaternion.h"
#include "math/Matrix4.h"

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

        Matrix4 uiTransform;

        Vector3 uiPosition;
        Vector3 uiScale;
        Quaternion uiRotation;

        float anchorPointLeft = 0;
        float anchorPointTop = 0;
        float anchorPointRight = 0;
        float anchorPointBottom = 0;

        int anchorOffsetLeft = 0;
        int anchorOffsetTop = 0;
        int anchorOffsetRight = 0;
        int anchorOffsetBottom = 0;

        AnchorPreset anchorPreset = AnchorPreset::NONE;
        bool usingAnchors = false;

        int containerBoxIndex = -1;

        Rect scissor = Rect(0, 0, 0, 0);
        bool ignoreScissor = false; // ignore parent scissor
        bool ignoreEvents = false;

        bool needUpdateSizes = false;
    };
    
}

#endif //UI_LAYOUT_COMPONENT_H