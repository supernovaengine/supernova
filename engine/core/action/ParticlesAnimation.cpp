//
// (c) 2021 Eduardo Doria.
//

#include "ParticlesAnimation.h"
#include "Ease.h"

using namespace Supernova;

ParticlesAnimation::ParticlesAnimation(Scene* scene): Action(scene){
    addComponent<ParticlesAnimationComponent>({});
}

void ParticlesAnimation::setLifeInitializer(float life){
    setLifeInitializer(life, life);
}

void ParticlesAnimation::setLifeInitializer(float minLife, float maxLife){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.lifeInitializer.minLife = minLife;
    partAnim.lifeInitializer.maxLife = maxLife;
}

void ParticlesAnimation::setPositionInitializer(Vector3 position){
    setPositionInitializer(position, position);
}

void ParticlesAnimation::setPositionInitializer(Vector3 minPosition, Vector3 maxPosition){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.positionInitializer.minPosition = minPosition;
    partAnim.positionInitializer.maxPosition = maxPosition;
}

void ParticlesAnimation::setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition){
    setPositionModifier(fromTime, toTime, fromPosition, toPosition, S_LINEAR);
}

void ParticlesAnimation::setPositionModifier(float fromTime, float toTime, Vector3 fromPosition, Vector3 toPosition, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.positionModifier.fromTime = fromTime;
    partAnim.positionModifier.toTime = toTime;
    partAnim.positionModifier.fromPosition = fromPosition;
    partAnim.positionModifier.toPosition = toPosition;
    partAnim.positionModifier.function = Ease::getFunction(functionType);
}

void ParticlesAnimation::setVelocityInitializer(Vector3 velocity){
    setVelocityInitializer(velocity, velocity);
}

void ParticlesAnimation::setVelocityInitializer(Vector3 minVelocity, Vector3 maxVelocity){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.velocityInitializer.minVelocity = minVelocity;
    partAnim.velocityInitializer.maxVelocity = maxVelocity;
}

void ParticlesAnimation::setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity){
    setVelocityModifier(fromTime, toTime, fromVelocity, toVelocity, S_LINEAR);
}

void ParticlesAnimation::setVelocityModifier(float fromTime, float toTime, Vector3 fromVelocity, Vector3 toVelocity, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.velocityModifier.fromTime = fromTime;
    partAnim.velocityModifier.toTime = toTime;
    partAnim.velocityModifier.fromVelocity = fromVelocity;
    partAnim.velocityModifier.toVelocity = toVelocity;
    partAnim.velocityModifier.function = Ease::getFunction(functionType);
}

void ParticlesAnimation::setSizeInitializer(float size){
    setSizeInitializer(size, size);
}

void ParticlesAnimation::setSizeInitializer(float minSize, float maxSize){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.sizeInitializer.minSize = minSize;
    partAnim.sizeInitializer.maxSize = maxSize;
}

void ParticlesAnimation::setSizeModifier(float fromTime, float toTime, float fromSize, float toSize){
    setSizeModifier(fromTime, toTime, fromSize, toSize, S_LINEAR);
}

void ParticlesAnimation::setSizeModifier(float fromTime, float toTime, float fromSize, float toSize, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.sizeModifier.fromTime = fromTime;
    partAnim.sizeModifier.toTime = toTime;
    partAnim.sizeModifier.fromSize = fromSize;
    partAnim.sizeModifier.toSize = toSize;
    partAnim.sizeModifier.function = Ease::getFunction(functionType);
}

void ParticlesAnimation::setSpriteIntializer(std::vector<int> frames){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.spriteInitializer.frames = frames;
}

void ParticlesAnimation::setSpriteIntializer(int minFrame, int maxFrame){
    if (minFrame <= maxFrame){
        ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();
        partAnim.spriteInitializer.frames.clear();
        for (int i = minFrame; i <= maxFrame; i++){
            partAnim.spriteInitializer.frames.push_back(i);
        }
    }
}

void ParticlesAnimation::setSpriteModifier(float fromTime, float toTime, std::vector<int> frames){
    setSpriteModifier(fromTime, toTime, frames, S_LINEAR);
}

void ParticlesAnimation::setSpriteModifier(float fromTime, float toTime, std::vector<int> frames, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.spriteModifier.fromTime = fromTime;
    partAnim.spriteModifier.toTime = toTime;
    partAnim.spriteModifier.frames = frames;
    partAnim.spriteModifier.function = Ease::getFunction(functionType);
}

void ParticlesAnimation::setRotationInitializer(float rotation){
    setRotationInitializer(rotation, rotation);
}

void ParticlesAnimation::setRotationInitializer(float minRotation, float maxRotation){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.rotationInitializer.minRotation = minRotation;
    partAnim.rotationInitializer.maxRotation = maxRotation;
}

void ParticlesAnimation::setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation){
    setRotationModifier(fromTime, toTime, fromRotation, toRotation, S_LINEAR);
}

void ParticlesAnimation::setRotationModifier(float fromTime, float toTime, float fromRotation, float toRotation, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.rotationModifier.fromTime = fromTime;
    partAnim.rotationModifier.toTime = toTime;
    partAnim.rotationModifier.fromRotation = fromRotation;
    partAnim.rotationModifier.toRotation = toRotation;
    partAnim.rotationModifier.function = Ease::getFunction(functionType);
}