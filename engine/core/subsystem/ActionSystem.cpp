#include "ActionSystem.h"

#include "Scene.h"

using namespace Supernova;


ActionSystem::ActionSystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<ActionComponent>());
}

void ActionSystem::actionStart(ActionComponent& action){
    action.onStart.call();
}

void ActionSystem::actionStop(ActionComponent& action){
    action.onStop.call();
}

void ActionSystem::actionPause(ActionComponent& action){
    action.onPause.call();
}

void ActionSystem::setSpriteTextureRect(SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    if (spriteanim.frameIndex < MAX_SPRITE_FRAMES){
        FrameData frameData = sprite.framesRect[spriteanim.frames[spriteanim.frameIndex]];
        if (frameData.active)
            sprite.textureRect = frameData.rect;
    }
}

void ActionSystem::spriteActionStart(SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    setSpriteTextureRect(sprite, spriteanim);
}

void ActionSystem::spriteActionStop(SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    spriteanim.frameIndex = 0;
    spriteanim.frameTimeIndex = 0;
    spriteanim.spriteFrameCount = 0;

    setSpriteTextureRect(sprite, spriteanim);
}

void ActionSystem::spriteActionUpdate(double dt, ActionComponent& action, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
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

        setSpriteTextureRect(sprite, spriteanim);
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

            Entity entity = actions->getEntity(i);
            Signature targetSignature = scene->getSignature(action.target);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);

                    spriteActionStart(sprite, spriteanim);

                }
            }
        }

		if (action.stopTrigger == true && (action.state == ActionState::Running || action.state == ActionState::Paused)){
            action.state = ActionState::Stopped;
            action.stopTrigger = false;
            actionStop(action);

            Entity entity = actions->getEntity(i);
            Signature targetSignature = scene->getSignature(action.target);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);

                    spriteActionStop(sprite, spriteanim);

                }
            }
        }

        if (action.pauseTrigger == true && action.state == ActionState::Running){
            action.state = ActionState::Paused;
            action.pauseTrigger = false;
            actionPause(action);
        }

        if ((action.state == ActionState::Running) && (action.target != NULL_ENTITY)){

            Entity entity = actions->getEntity(i);
            Signature targetSignature = scene->getSignature(action.target);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);

                    spriteActionUpdate(dt, action, sprite, spriteanim);

                }
            }
        }

	}
}

void ActionSystem::entityDestroyed(Entity entity){

}