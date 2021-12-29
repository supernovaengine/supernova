//
// (c) 2021 Eduardo Doria.
//

#ifndef COLORACTION_COMPONENT_H
#define COLORACTION_COMPONENT_H

#include "math/Vector3.h"

namespace Supernova{

    struct ColorActionComponent{
        Vector3 endColor;
        Vector3 startColor;

        bool useSRGB = true;
    };

}

#endif //COLORACTION_COMPONENT_H