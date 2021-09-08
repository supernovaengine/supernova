#ifndef ANIMETATEDSPRITE_COMPONENT_H
#define ANIMETATEDSPRITE_COMPONENT_H

#include "Supernova.h"
#include "math/Rect.h"

namespace Supernova{

    struct FrameData{
        std::string name;
        Rect rect;
        bool active = false;
    };
        

    struct AnimatedSpriteComponent{
        FrameData framesRect[MAX_SPRITE_FRAMES];
    };
    
}

#endif //ANIMETATEDSPRITE_COMPONENT_H