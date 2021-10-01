//
// (c) 2021 Eduardo Doria.
//

#ifndef COLORACTION_COMPONENT_H
#define COLORACTION_COMPONENT_H

#include "math/Vector4.h"

namespace Supernova{

    struct ColorActionComponent{
        //sRGB color
        Vector4 endColor;
        Vector4 startColor;
    };

}

#endif //COLORACTION_COMPONENT_H