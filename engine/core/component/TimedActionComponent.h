// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TIMEDACTION_COMPONENT_H
#define TIMEDACTION_COMPONENT_H

#include "action/Ease.h"

namespace doriax{

    struct DORIAX_API TimedActionComponent{
        float time = 0;
        float value = 0;
        
        float duration = 0;
        bool loop = false;

        Ease function;
    };

}

#endif //TIMEDACTION_COMPONENT_H