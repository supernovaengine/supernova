//
// (c) 2021 Eduardo Doria.
//

#include "ParticlesAnimation.h"
#include "Ease.h"

using namespace Supernova;

ParticlesAnimation::ParticlesAnimation(Scene* scene): Action(scene){
    addComponent<ParticlesAnimationComponent>({});
}

void ParticlesAnimation::setVelocityInitializer(Vector3 velocity){
    setVelocityInitializer(velocity, velocity);
}

void ParticlesAnimation::setVelocityInitializer(Vector3 minVelocity, Vector3 maxVelocity){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.velocityInitializer.minVelocity = minVelocity;
    partAnim.velocityInitializer.maxVelocity = maxVelocity;
}

void ParticlesAnimation::setVelocityModifier(float fromLife, float toLife, Vector3 fromVelocity, Vector3 toVelocity){
    setVelocityModifier(fromLife, toLife, fromVelocity, toVelocity, S_LINEAR);
}

void ParticlesAnimation::setVelocityModifier(float fromLife, float toLife, Vector3 fromVelocity, Vector3 toVelocity, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.velocityModifier.fromLife = fromLife;
    partAnim.velocityModifier.toLife = toLife;
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

void ParticlesAnimation::setSizeModifier(float fromLife, float toLife, float fromSize, float toSize){
    setSizeModifier(fromLife, toLife, fromSize, toSize, S_LINEAR);
}

void ParticlesAnimation::setSizeModifier(float fromLife, float toLife, float fromSize, float toSize, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.sizeModifier.fromLife = fromLife;
    partAnim.sizeModifier.toLife = toLife;
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

void ParticlesAnimation::setSpriteModifier(float fromLife, float toLife, std::vector<int> frames){
    setSpriteModifier(fromLife, toLife, frames, S_LINEAR);
}

void ParticlesAnimation::setSpriteModifier(float fromLife, float toLife, std::vector<int> frames, int functionType){
    ParticlesAnimationComponent& partAnim = getComponent<ParticlesAnimationComponent>();

    partAnim.spriteModifier.fromLife = fromLife;
    partAnim.spriteModifier.toLife = toLife;
    partAnim.spriteModifier.frames = frames;
    partAnim.spriteModifier.function = Ease::getFunction(functionType);
}