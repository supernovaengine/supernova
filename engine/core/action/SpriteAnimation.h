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

        void setAnimation(std::vector<int> framesTime, std::vector<int> frames, bool loop);
       /* setAnimation(std::vector<int> framesTime, int startFrame, int endFrame, bool loop);
        setAnimation(int interval, int startFrame, int endFrame, bool loop);
        setAnimation(int interval, std::vector<int> frames, bool loop);
        */
    };
}

#endif //SPRITEANIMATION_H