// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include "Action.h"

namespace doriax{
    class DORIAX_API SpriteAnimation: public Action{

    public:
        SpriteAnimation(Scene* scene);
        SpriteAnimation(Scene* scene, Entity entity);

        void setAnimation(std::vector<int> frames, std::vector<int> framesTime, bool loop);
        void setAnimation(int startFrame, int endFrame, int interval, bool loop);
    };
}

#endif //SPRITEANIMATION_H