//
// (c) 2024 Eduardo Doria.
//

#ifndef KEYFRAMETRACKS_COMPONENT_H
#define KEYFRAMETRACKS_COMPONENT_H

#include "Engine.h"

namespace Supernova{

    struct KeyframeTracksComponent{
        std::vector<float> times;
        int index = 0;
        float interpolation = 0;
    };

}

#endif //KEYFRAMETRACKS_COMPONENT_H