//
// (c) 2024 Eduardo Doria.
//

#ifndef SCROLLBAR_COMPONENT_H
#define SCROLLBAR_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace Supernova{

    enum class ScrollbarType{
        VERTICAL,
        HORIZONTAL
    };

    struct ScrollbarComponent{
        Entity bar = NULL_ENTITY;
        ScrollbarType type = ScrollbarType::VERTICAL;

        int barSize = 50;

        bool barPointerDown = false;
        float barPointerPos = -1;

        bool needUpdateScrollbar = true;
    };

}

#endif //SCROLLBAR_COMPONENT_H