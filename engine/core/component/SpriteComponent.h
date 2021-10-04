#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "util/FrameData.h"
#include "Supernova.h"

namespace Supernova{

    struct SpriteComponent{
        int width = 200;
        int height = 200;

        FrameData framesRect[MAX_SPRITE_FRAMES];
    };
    
}

#endif //SPRITE_COMPONENT_H