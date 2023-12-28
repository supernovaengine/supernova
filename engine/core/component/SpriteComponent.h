#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "util/FrameData.h"
#include "Engine.h"

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
        int width = 0;
        int height = 0;

        bool automaticFlipY = true;
        bool flipY = false;

        float textureCutFactor = 0.0;

        FrameData framesRect[MAX_SPRITE_FRAMES];

        PivotPreset pivotPreset = PivotPreset::BOTTOM_LEFT;

        bool needUpdateSprite = true;
    };
    
}

#endif //SPRITE_COMPONENT_H