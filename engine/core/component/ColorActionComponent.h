// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef COLORACTION_COMPONENT_H
#define COLORACTION_COMPONENT_H

#include "math/Vector3.h"

namespace doriax{

    struct DORIAX_API ColorActionComponent{
        Vector3 endColor;
        Vector3 startColor;

        bool useSRGB = true;
    };

}

#endif //COLORACTION_COMPONENT_H