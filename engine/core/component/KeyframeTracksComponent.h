#ifndef KEYFRAMETRACKS_COMPONENT_H
#define KEYFRAMETRACKS_COMPONENT_H

#include "Supernova.h"

namespace Supernova{

    struct KeyframeTracksComponent{
        std::vector<float> times;
        int index = 0;
        float progress = 0;
    };

}

#endif //KEYFRAMETRACKS_COMPONENT_H