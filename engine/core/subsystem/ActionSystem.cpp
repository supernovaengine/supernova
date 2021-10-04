#include "ActionSystem.h"

#include "Scene.h"
#include "math/Color.h"

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

void ActionSystem::setSpriteTextureRect(MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    if (spriteanim.frameIndex < MAX_SPRITE_FRAMES){
        FrameData frameData = sprite.framesRect[spriteanim.frames[spriteanim.frameIndex]];
        if (frameData.active)
            mesh.submeshes[0].textureRect = frameData.rect;
    }
}

void ActionSystem::spriteActionStart(MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    setSpriteTextureRect(mesh, sprite, spriteanim);
}

void ActionSystem::spriteActionStop(MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    spriteanim.frameIndex = 0;
    spriteanim.frameTimeIndex = 0;
    spriteanim.spriteFrameCount = 0;

    setSpriteTextureRect(mesh, sprite, spriteanim);
}

void ActionSystem::spriteActionUpdate(double dt, ActionComponent& action, MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
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

        setSpriteTextureRect(mesh, sprite, spriteanim);
    }
}

void ActionSystem::timedActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, EaseComponent& ease){
    timedaction.timecount += dt;

    if ((timedaction.time == 1) && !timedaction.loop){
        action.stopTrigger = true;
        //onFinish.call(object);
    } else {
        if (timedaction.duration >= 0) {

            if (timedaction.timecount >= timedaction.duration){
                if (!timedaction.loop){
                    timedaction.timecount = timedaction.duration;
                }else{
                    timedaction.timecount -= timedaction.duration;
                }
            }

            timedaction.time = timedaction.timecount / timedaction.duration;
        }

        timedaction.value = ease.function.call(timedaction.time);
        //Log::Debug("step time %f value %f \n", timedaction.time, timedaction.value);
    }
}

void ActionSystem::positionActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, PositionActionComponent& posaction, Transform& transform){
    Vector3 position = (posaction.endPosition - posaction.startPosition) * timedaction.value;
    transform.position = posaction.startPosition + position;
    transform.needUpdate = true;
}

void ActionSystem::rotationActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, RotationActionComponent& rotaction, Transform& transform){
    transform.rotation = transform.rotation.slerp(timedaction.value, rotaction.startRotation, rotaction.endRotation);
    transform.needUpdate = true;
}

void ActionSystem::scaleActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ScaleActionComponent& scaleaction, Transform& transform){
    Vector3 scale = (scaleaction.endScale - scaleaction.startScale) * timedaction.value;
    transform.scale = scaleaction.startScale + scale;
    transform.needUpdate = true;
}

void ActionSystem::colorActionSpriteUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ColorActionComponent& coloraction, MeshComponent& mesh){
    Vector4 color = (coloraction.endColor - coloraction.startColor) * timedaction.value;
    mesh.submeshes[0].material.baseColorFactor = Color::sRGBToLinear(coloraction.startColor + color);
}

void ActionSystem::load(){

}

void ActionSystem::draw(){

}

void ActionSystem::update(double dt){
    auto actions = scene->getComponentArray<ActionComponent>();
    for (int i = 0; i < actions->size(); i++){
		ActionComponent& action = actions->getComponentFromIndex(i);

        // Action start
		if (action.startTrigger == true && (action.state == ActionState::Stopped || action.state == ActionState::Paused)){
            action.state = ActionState::Running;
            action.startTrigger = false;
            actionStart(action);

            Entity entity = actions->getEntity(i);
            Signature targetSignature = scene->getSignature(action.target);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>()) && targetSignature.test(scene->getComponentType<MeshComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);
                    MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                    spriteActionStart(mesh, sprite, spriteanim);

                }
            }
        }

        // Action stop
		if (action.stopTrigger == true && (action.state == ActionState::Running || action.state == ActionState::Paused)){
            action.state = ActionState::Stopped;
            action.stopTrigger = false;
            actionStop(action);

            Entity entity = actions->getEntity(i);
            Signature targetSignature = scene->getSignature(action.target);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>()) && targetSignature.test(scene->getComponentType<MeshComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);
                    MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                    spriteActionStop(mesh, sprite, spriteanim);

                }
            }
        }

        // Action pause
        if (action.pauseTrigger == true && action.state == ActionState::Running){
            action.state = ActionState::Paused;
            action.pauseTrigger = false;
            actionPause(action);
        }

        // Action update
        if ((action.state == ActionState::Running) && (action.target != NULL_ENTITY)){

            Entity entity = actions->getEntity(i);
            Signature targetSignature = scene->getSignature(action.target);
            Signature signature = scene->getSignature(entity);

            //Sprite animation
            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>()) && targetSignature.test(scene->getComponentType<MeshComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);
                    MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                    spriteActionUpdate(dt, action, mesh, sprite, spriteanim);

                }
            }

            if (signature.test(scene->getComponentType<TimedActionComponent>()) && signature.test(scene->getComponentType<EaseComponent>())){
                TimedActionComponent& timedaction = scene->getComponent<TimedActionComponent>(entity);
                EaseComponent& ease = scene->getComponent<EaseComponent>(entity);

                timedActionUpdate(dt, action, timedaction, ease);

                //Transform animation
                if (targetSignature.test(scene->getComponentType<Transform>())){
                    Transform& transform = scene->getComponent<Transform>(action.target);

                    if (signature.test(scene->getComponentType<PositionActionComponent>())){
                        PositionActionComponent& posaction = scene->getComponent<PositionActionComponent>(entity);

                        positionActionUpdate(dt, action, timedaction, posaction, transform);
                    }

                    if (signature.test(scene->getComponentType<RotationActionComponent>())){
                        RotationActionComponent& rotaction = scene->getComponent<RotationActionComponent>(entity);

                        rotationActionUpdate(dt, action, timedaction, rotaction, transform);
                    }

                    if (signature.test(scene->getComponentType<ScaleActionComponent>())){
                        ScaleActionComponent& scaleaction = scene->getComponent<ScaleActionComponent>(entity);

                        scaleActionUpdate(dt, action, timedaction, scaleaction, transform);
                    }

                }

                //Color animation
                if (signature.test(scene->getComponentType<ColorActionComponent>())){
                    ColorActionComponent& coloraction = scene->getComponent<ColorActionComponent>(entity);

                    if (targetSignature.test(scene->getComponentType<MeshComponent>())){
                        MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                        colorActionSpriteUpdate(dt, action, timedaction, coloraction, mesh);
                    }

                }
            }
        }

	}
}

void ActionSystem::entityDestroyed(Entity entity){

}