#include "ActionSystem.h"

#include "Scene.h"

using namespace Supernova;


ActionSystem::ActionSystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<ActionComponent>());
}

void ActionSystem::actionStart(ActionComponent& action){

}

void ActionSystem::actionStop(ActionComponent& action){

}

void ActionSystem::actionPause(ActionComponent& action){

}

void ActionSystem::spriteAction(double dt, ActionComponent& action, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    spriteanim.spriteFrameCount += dt * 1000;
    while (spriteanim.spriteFrameCount >= spriteanim.framesTime[spriteanim.frameTimeIndex]) {

        spriteanim.spriteFrameCount -= spriteanim.framesTime[spriteanim.frameTimeIndex];

        spriteanim.frameIndex++;
        spriteanim.frameTimeIndex++;

        if (spriteanim.frameIndex == spriteanim.framesSize - 1) {
            if (!spriteanim.loop) {
                action.stopTrigger = true;
            }
        }

        if (spriteanim.frameIndex >= spriteanim.framesSize)
            spriteanim.frameIndex = 0;
        
        if (spriteanim.frameTimeIndex >= spriteanim.framesTimeSize)
            spriteanim.frameTimeIndex = 0;
    }

    if (spriteanim.frameIndex < MAX_SPRITE_FRAMES){
        FrameData frameData = sprite.framesRect[spriteanim.frames[spriteanim.frameIndex]];
        if (frameData.active)
            sprite.textureRect = frameData.rect;
    }
}

void ActionSystem::load(){

}

void ActionSystem::draw(){

}

void ActionSystem::update(double dt){
    auto actions = scene->getComponentArray<ActionComponent>();
    for (int i = 0; i < actions->size(); i++){
		ActionComponent& action = actions->getComponentFromIndex(i);
		
		if (action.startTrigger == true && (action.state == ActionState::Stopped || action.state == ActionState::Paused)){
            action.state = ActionState::Running;
            action.startTrigger = false;
            actionStart(action);
        }

		if (action.stopTrigger == true && (action.state == ActionState::Running || action.state == ActionState::Paused)){
            action.state = ActionState::Stopped;
            action.stopTrigger = false;
            actionStop(action);
        }

        if (action.pauseTrigger == true && action.state == ActionState::Running){
            action.state = ActionState::Paused;
            action.pauseTrigger = false;
            actionPause(action);
        }

        Entity target = action.target;

        if ((action.state == ActionState::Running) && (target != NULL_ENTITY)){

            Signature targetSignature = scene->getSignature(target);
            Entity entity = actions->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(target);

                    spriteAction(dt, action, sprite, spriteanim);

                }
            }
        }

	}
}

void ActionSystem::entityDestroyed(Entity entity){

}