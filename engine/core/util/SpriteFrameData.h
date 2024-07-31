//
// (c) 2024 Eduardo Doria.
//

#ifndef SPRITEFRAMEDATA_H
#define SPRITEFRAMEDATA_H

#include "math/Rect.h"

namespace Supernova{

    struct SpriteFrameData{
        bool active = false;
        std::string name;
        Rect rect;
    };

}

#endif //SPRITEFRAMEDATA_H