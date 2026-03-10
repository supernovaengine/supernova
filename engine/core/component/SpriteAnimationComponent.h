//
// (c) 2024 Eduardo Doria.
//

#ifndef SPRITEANIMATION_COMPONENT_H
#define SPRITEANIMATION_COMPONENT_H

#include "Engine.h"
#include "util/HybridArray.h"

namespace Supernova{

    struct SUPERNOVA_API SpriteAnimationComponent{
        std::string name;

        bool loop = true;

        unsigned int framesTimeSize;
        HybridArray<int, MAX_SPRITE_FRAMES> framesTime;

        unsigned int framesSize;
        HybridArray<int, MAX_SPRITE_FRAMES> frames;

        int frameIndex;
        int frameTimeIndex;

        unsigned int spriteFrameCount;
    };
    
}

#endif //SPRITEANIMATION_COMPONENT_H