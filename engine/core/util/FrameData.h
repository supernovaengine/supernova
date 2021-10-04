#ifndef FRAMEDATA_H
#define FRAMEDATA_H

#include "math/Rect.h"

namespace Supernova{

    struct FrameData{
        bool active = false;
        std::string name;
        Rect rect;
    };

}

#endif //FRAMEDATA_H