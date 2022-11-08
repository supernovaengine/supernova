#ifndef UI_LAYOUT_COMPONENT_H
#define UI_LAYOUT_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace Supernova{

    struct UILayoutComponent{
        // UI part
        int width = 0;
        int height = 0;

        float anchorLeft = 0.5;
        float anchorTop = 0.5;
        float anchorRight = 0.5;
        float anchorBottom = 0.5;

        float marginLeft = -74;
        float marginTop = -52;
        float marginRight = 74;
        float marginBottom = 52;

        FunctionSubscribe<void()> onMouseMove;
        bool mouseMoved = false;

        bool focused = false;

        bool needUpdateAnchors = true;
    };
    
}

#endif //UI_LAYOUT_COMPONENT_H