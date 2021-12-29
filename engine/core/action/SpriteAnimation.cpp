//
// (c) 2021 Eduardo Doria.
//

#include "SpriteAnimation.h"

using namespace Supernova;

SpriteAnimation::SpriteAnimation(Scene* scene): Action(scene){
    addComponent<SpriteAnimationComponent>({});
}

void SpriteAnimation::setAnimation(std::vector<int> frames, std::vector<int> framesTime, bool loop){
    SpriteAnimationComponent& spriteanim = getComponent<SpriteAnimationComponent>();

    spriteanim.framesTimeSize = framesTime.size();
    for (int i = 0; i < framesTime.size(); i++){
        spriteanim.framesTime[i] = framesTime[i];
    }

    spriteanim.framesSize = frames.size();
    for (int i = 0; i < frames.size(); i++){
        spriteanim.frames[i] = frames[i];
    }

    spriteanim.loop = loop;
}

void SpriteAnimation::setAnimation(int startFrame, int endFrame, int interval, bool loop){
    std::vector<int> frames;
    std::vector<int> framesTime;

    if (startFrame > 0 || endFrame > 0) {
        for (int i=startFrame; i<=endFrame; i++){
            frames.push_back(i);
            framesTime.push_back(interval);
        }
    }

    setAnimation(frames, framesTime, loop);
}