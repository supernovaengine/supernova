#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "util/FrameData.h"
#include "Supernova.h"

namespace Supernova{

    struct SpriteComponent{
        int width = 100;
        int height = 100;

        FrameData framesRect[MAX_SPRITE_FRAMES];

        bool automaticFlipY = true;
        bool flipY = false;

        bool needUpdateSprite = true;
    };
    
}

#endif //SPRITE_COMPONENT_H