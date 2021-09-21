#ifndef EASE_COMPONENT_H
#define EASE_COMPONENT_H

#include "util/Function.h"

namespace Supernova{

    struct EaseComponent{

        Function<float(float)> function;
        
    };

}

#endif //EASE_COMPONENT_H