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

        int marginLeft = 0;
        int marginTop = 0;
        int marginRight = 0;
        int marginBottom = 0;

        FunctionSubscribe<void()> onMouseMove;
        bool mouseMoved = false;

        bool focused = false;

        bool needUpdateSizes = false;
        bool needUpdateAnchors = false;
    };
    
}

#endif //UI_LAYOUT_COMPONENT_H