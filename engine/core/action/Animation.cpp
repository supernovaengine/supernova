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

bool Animation::isLoop() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.loop;
}

void Animation::setLoop(bool loop){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.loop = loop;
}

void Animation::setEndTime(float endTime){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.endTime = endTime;
}

float Animation::getEndTime() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.endTime;
}

bool Animation::isOwnedActions() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.ownedActions;
}

void Animation::setOwnedActions(bool ownedActions){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.ownedActions = ownedActions;
}

const std::string &Animation::getName() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.name;
}

void Animation::setName(const std::string &name) {
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.name = name;
}

void Animation::addActionFrame(float startTime, float endTime, Entity action, Entity target){
    AnimationComponent& animation = getComponent<AnimationComponent>();
    ActionComponent& actioncomp = getComponent<ActionComponent>();

    ActionFrame actionFrame;

    actionFrame.startTime = startTime;
    actionFrame.endTime = endTime;
    actionFrame.action = action;

    actioncomp.target = target;

    animation.actions.push_back(actionFrame);

    if ((animation.endTime == 0) || (animation.endTime < endTime)){
        animation.endTime = endTime;
    }
}

void Animation::addActionFrame(float startTime, Entity timedaction, Entity target){
    TimedActionComponent& timedactioncomp = scene->getComponent<TimedActionComponent>(timedaction);

    addActionFrame(startTime, startTime + timedactioncomp.duration, timedaction, target);
}

ActionFrame Animation::getActionFrame(unsigned int index){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    try{
        return animation.actions.at(index);
    }catch (const std::out_of_range& e){
		Log::Error("Retrieving non-existent action: %s", e.what());
		throw;
	}
}

void Animation::clearActionFrames(){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    if (animation.ownedActions){
        for (int i = 0; i < animation.actions.size(); i++){
            scene->destroyEntity(animation.actions[i].action);
        }
    }
    animation.actions.clear();
}
