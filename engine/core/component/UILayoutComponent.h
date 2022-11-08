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

        float marginLeft = 0;
        float marginTop = 0;
        float marginRight = 0;
        float marginBottom = 0;

        FunctionSubscribe<void()> onMouseMove;
        bool mouseMoved = false;

        bool focused = false;

        bool needUpdateSizes = false;
        bool needUpdateAnchors = false;
    };
    
}

#endif //UI_LAYOUT_COMPONENT_H