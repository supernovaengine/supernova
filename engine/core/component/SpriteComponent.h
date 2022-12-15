#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "util/FrameData.h"
#include "Supernova.h"

namespace Supernova{

    enum class PivotPreset{
        CENTER,
        TOP_CENTER,
        BOTTOM_CENTER,
        LEFT_CENTER,
        RIGHT_CENTER,
        TOP_LEFT,
        BOTTOM_LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT
    };

    struct SpriteComponent{
        int width = 100;
        int height = 100;

        FrameData framesRect[MAX_SPRITE_FRAMES];

        bool automaticFlipY = true;
        bool flipY = false;

        PivotPreset pivot = PivotPreset::BOTTOM_LEFT;

        bool needUpdateSprite = true;
    };
    
}

#endif //SPRITE_COMPONENT_H