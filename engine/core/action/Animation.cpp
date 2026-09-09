// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Animation.h"

#include "component/AnimationComponent.h"
#include "subsystem/ActionSystem.h"

#include <algorithm>

using namespace doriax;

Animation::Animation(Scene* scene): Action(scene){
    addComponent<AnimationComponent>();
}

Animation::Animation(Scene* scene, Entity entity): Action(scene, entity){

}

Animation::~Animation(){

}

Animation::Animation(const Animation& rhs): Action(rhs){
    
}

Animation& Animation::operator=(const Animation& rhs){
    Action::operator =(rhs);

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

void Animation::fadeIn(float duration){
    AnimationComponent& animation = getComponent<AnimationComponent>();
    ActionComponent& action = getComponent<ActionComponent>();

    const bool alreadyRunning = action.state == ActionState::Running;

    // A new fade-in supersedes a pending stop/pause and must not leave a start
    // trigger behind when the animation is already running. Otherwise that stale
    // trigger can restart the clip after a later fade-out reaches zero.
    action.startTrigger = false;
    action.stopTrigger = false;
    action.pauseTrigger = false;

    animation.weight = std::clamp(animation.weight, 0.0f, 1.0f);

    if (duration <= 0.0f){
        animation.weight = 1.0f;
        animation.fadeTarget = 1.0f;
        animation.fadeSpeed = 0.0f;
        animation.stopOnFadeOut = false;
    }else{
        if (!alreadyRunning){
            animation.weight = 0.0f;
        }

        animation.fadeTarget = 1.0f;
        animation.fadeSpeed = (animation.weight < 1.0f ? 1.0f - animation.weight : 0.0f) / duration;
        animation.stopOnFadeOut = false;
    }

    if (!alreadyRunning){
        start();
    }
}

void Animation::fadeOut(float duration){
    AnimationComponent& animation = getComponent<AnimationComponent>();
    ActionComponent& action = getComponent<ActionComponent>();

    // A clip that has not reached the action-system tick yet can still have a
    // pending start. Cancel it directly instead of allowing both clips to start.
    if (action.state == ActionState::Stopped){
        action.startTrigger = false;
        action.stopTrigger = false;
        action.pauseTrigger = false;
        animation.weight = 0.0f;
        animation.fadeTarget = 0.0f;
        animation.fadeSpeed = 0.0f;
        animation.stopOnFadeOut = false;
        return;
    }

    // Paused animations cannot advance a fade, so remove them immediately.
    if (action.state == ActionState::Paused){
        action.startTrigger = false;
        action.pauseTrigger = false;
        animation.weight = 0.0f;
        animation.fadeTarget = 0.0f;
        animation.fadeSpeed = 0.0f;
        animation.stopOnFadeOut = false;
        stop();
        return;
    }

    action.startTrigger = false;
    action.stopTrigger = false;
    action.pauseTrigger = false;

    if (duration <= 0.0f){
        stop();
        return;
    }

    animation.fadeTarget = 0.0f;
    animation.fadeSpeed = (animation.weight > 0.0f ? animation.weight : 1.0f) / duration;
    animation.stopOnFadeOut = true;
}

void Animation::start(float fadeInDuration){
    fadeIn(fadeInDuration);
}

float Animation::getBlendWeight() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.weight;
}

void Animation::setBlendWeight(float weight){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.weight = std::clamp(weight, 0.0f, 1.0f);
    animation.fadeSpeed = 0.0f;
    animation.fadeTarget = animation.weight;
}

float Animation::getDefaultFadeTime() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.defaultFadeTime;
}

void Animation::setDefaultFadeTime(float time){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.defaultFadeTime = time;
}

bool Animation::isOwnedActions() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.ownedActions;
}

void Animation::setOwnedActions(bool ownedActions){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.ownedActions = ownedActions;
}

const float &Animation::getDuration() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.duration;
}

void Animation::setDuration(const float &duration){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    animation.duration = duration;
}

void Animation::addActionFrame(float startTime, float duration, Entity action, Entity target){
    if (scene->getSystem<ActionSystem>()->isAnimationReachable(action, entity)){
        Log::error("Cannot add entity %u as action of animation %u: it would create an animation cycle", action, entity);
        return;
    }

    AnimationComponent& animation = getComponent<AnimationComponent>();
    ActionComponent& actioncomp = getComponent<ActionComponent>();

    ActionFrame actionFrame;

    actionFrame.startTime = startTime;
    actionFrame.duration = duration;
    actionFrame.action = action;

    animation.actions.push_back(actionFrame);

    if (target != NULL_ENTITY){
        actioncomp.target = target;

        ActionComponent& childAction = scene->getComponent<ActionComponent>(action);

        childAction.target = target;
    }
}

size_t Animation::getActionFrameSize() const{
    AnimationComponent& animation = getComponent<AnimationComponent>();

    return animation.actions.size();
}

void Animation::addActionFrame(float startTime, Entity timedaction, Entity target){
    // Duration <= 0 means auto: resolved from the action's own duration
    addActionFrame(startTime, 0, timedaction, target);
}

void Animation::addActionFrame(float startTime, float duration, Entity action){
    addActionFrame(startTime, duration, action, getTarget());
}

void Animation::addActionFrame(float startTime, Entity timedaction){
    addActionFrame(startTime, timedaction, getTarget());
}

ActionFrame& Animation::getActionFrame(unsigned int index){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    try{
        return animation.actions.at(index);
    }catch (const std::out_of_range& e){
        Log::error("Retrieving non-existent action: %s", e.what());
        throw;
	}
}

void Animation::setActionFrameStartTime(unsigned int index, float startTime){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    try{
        animation.actions.at(index).startTime = startTime;
    }catch (const std::out_of_range& e){
        Log::error("Retrieving non-existent action: %s", e.what());
        throw;
	}
}

void Animation::setActionFrameDuration(unsigned int index, float duration){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    try{
        animation.actions.at(index).duration = duration;
    }catch (const std::out_of_range& e){
        Log::error("Retrieving non-existent action: %s", e.what());
        throw;
	}
}

void Animation::setActionFrameEntity(unsigned int index, Entity action){
    AnimationComponent& animation = getComponent<AnimationComponent>();

    try{
        animation.actions.at(index).action = action;
    }catch (const std::out_of_range& e){
        Log::error("Retrieving non-existent action: %s", e.what());
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
