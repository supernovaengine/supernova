//
// (c) 2022 Eduardo Doria.
//

#include "Animation.h"

#include "component/AnimationComponent.h"

using namespace Supernova;

Animation::Animation(Scene* scene): Action(scene){
    addComponent<AnimationComponent>({});
}

Animation::Animation(Scene* scene, Entity entity): Action(scene, entity){

}

Animation::~Animation(){

}

Animation::Animation(const Animation& rhs): Action(rhs){
    
}

Animation& Animation::operator=(const Animation& rhs){

    return *this;
}

bool Animation::isLoop(){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.loop;
}

void Animation::setLoop(bool loop){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.loop = loop;
}

void Animation::setStartFrame(int frameIndex){

}

void Animation::setStartTime(float startTime){
    AnimationComponent& animation = getComponent<AnimationComponent>();
    ActionComponent& action = getComponent<ActionComponent>();

    animation.startTime = startTime;
    if (!isRunning())
        action.timecount = startTime;
}

float Animation::getStartTime(){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.startTime;
}