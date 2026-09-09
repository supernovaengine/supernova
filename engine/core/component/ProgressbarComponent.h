// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef PROGRESSBAR_COMPONENT_H
#define PROGRESSBAR_COMPONENT_H

#include "ecs/Entity.h"

namespace doriax{

    enum class ProgressbarType{
        VERTICAL,
        HORIZONTAL
    };

    struct DORIAX_API ProgressbarComponent{
        Entity fill = NULL_ENTITY;
        ProgressbarType type = ProgressbarType::HORIZONTAL;

        float value = 0; // from 0 to 1

        int fillMarginLeft = 0;
        int fillMarginRight = 0;
        int fillMarginTop = 0;
        int fillMarginBottom = 0;

        bool needUpdateProgressbar = true;
    };

}

#endif //PROGRESSBAR_COMPONENT_H
