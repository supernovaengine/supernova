//
// (c) 2024 Eduardo Doria.
//

#ifndef SCROLLBAR_COMPONENT_H
#define SCROLLBAR_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace Supernova{

    struct ScrollbarComponent{
        Entity bar = NULL_ENTITY;
        int barSize = 50;
        bool barPointerDown = false;
        float barPointerPos = -1;

        bool needUpdateScrollbar = true;
    };

}

#endif //SCROLLBAR_COMPONENT_H