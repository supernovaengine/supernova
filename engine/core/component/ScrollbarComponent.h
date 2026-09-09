// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SCROLLBAR_COMPONENT_H
#define SCROLLBAR_COMPONENT_H

#include "util/FunctionSubscribe.h"
#include "ecs/Entity.h"

namespace doriax{

    enum class ScrollbarType{
        VERTICAL,
        HORIZONTAL
    };

    struct DORIAX_API ScrollbarComponent{
        Entity bar = NULL_ENTITY;
        ScrollbarType type = ScrollbarType::VERTICAL;

        FunctionSubscribe<void(float)> onChange;

        float barSize = 0.5;  // from 0 to 1
        float step = 0; // from 0 to 1

        int barMarginLeft = 0;
        int barMarginRight = 0;
        int barMarginTop = 0;
        int barMarginBottom = 0;

        bool barPointerDown = false;
        float barPointerPos = -1;

        bool needUpdateScrollbar = true;
    };

}

#endif //SCROLLBAR_COMPONENT_H