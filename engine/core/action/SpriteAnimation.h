//
// (c) 2021 Eduardo Doria.
//

#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include "Action.h"

namespace Supernova{
    class SpriteAnimation: public Action{

    public:
        SpriteAnimation(Scene* scene);

        void setAnimation(std::vector<int> frames, std::vector<int> framesTime, bool loop);
        void setAnimation(int startFrame, int endFrame, int interval, bool loop);
    };
}

#endif //SPRITEANIMATION_H