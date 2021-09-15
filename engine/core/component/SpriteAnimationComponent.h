#ifndef SPRITEANIMATION_COMPONENT_H
#define SPRITEANIMATION_COMPONENT_H

#include "Supernova.h"

namespace Supernova{

    struct SpriteAnimationComponent{
        std::string name;

        bool loop = true;

        unsigned int framesTimeSize;
        int framesTime[MAX_SPRITE_FRAMES];

        unsigned int framesSize;
        int frames[MAX_SPRITE_FRAMES];

        int frameIndex;
        int frameTimeIndex;

        unsigned int spriteFrameCount;
    };
    
}

#endif //SPRITEANIMATION_COMPONENT_H