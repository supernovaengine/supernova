//
// (c) 2024 Eduardo Doria.
//

#include "ActionSystem.h"

#include "Scene.h"
#include "util/Color.h"
#include "util/Angle.h"

using namespace Supernova;


ActionSystem::ActionSystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<ActionComponent>());
}

void ActionSystem::actionStart(Entity entity){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);
    Signature signature = scene->getSignature(entity);

    actionComponentStart(action);

    if (action.target != NULL_ENTITY){
        Signature targetSignature = scene->getSignature(action.target);

        if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
            SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
            if (targetSignature.test(scene->getComponentType<SpriteComponent>()) && targetSignature.test(scene->getComponentType<MeshComponent>())){
                SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);
                MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                spriteActionStart(mesh, sprite, spriteanim);

            }
        }

        if (signature.test(scene->getComponentType<ParticlesComponent>())){
            ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);
            if (targetSignature.test(scene->getComponentType<MeshComponent>()) && !targetSignature.test(scene->getComponentType<InstancedMeshComponent>())){
                MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);
                scene->addComponent<InstancedMeshComponent>(action.target, {});
                targetSignature = scene->getSignature(action.target);
                if (mesh.loaded)
                    mesh.needReload = true;
            }
            if (targetSignature.test(scene->getComponentType<InstancedMeshComponent>()) ){
                InstancedMeshComponent& instmesh = scene->getComponent<InstancedMeshComponent>(action.target);

                particleActionStart(particles, instmesh);

            }
        }

        if (signature.test(scene->getComponentType<ParticlesAnimationComponent>())){
            ParticlesAnimationComponent& partanim = scene->getComponent<ParticlesAnimationComponent>(entity);
            if (targetSignature.test(scene->getComponentType<PointParticlesComponent>()) ){
                PointParticlesComponent& particles = scene->getComponent<PointParticlesComponent>(action.target);

                particleActionStart(partanim, particles);

            }
        }
    }
}

void ActionSystem::actionComponentStart(ActionComponent& action){
    action.state = ActionState::Running;
    action.startTrigger = false;

    action.onStart.call();
}

void ActionSystem::actionStop(Entity entity){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);
    Signature signature = scene->getSignature(entity);

    actionComponentStop(action);

    if (signature.test(scene->getComponentType<TimedActionComponent>())){
        TimedActionComponent& timedaction = scene->getComponent<TimedActionComponent>(entity);

        timedActionStop(timedaction);
    }

    if (action.target != NULL_ENTITY){
        Signature targetSignature = scene->getSignature(action.target);

        if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
            SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
            if (targetSignature.test(scene->getComponentType<SpriteComponent>()) && targetSignature.test(scene->getComponentType<MeshComponent>())){
                SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);
                MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                spriteActionStop(mesh, sprite, spriteanim);

            }
        }
    }
}

void ActionSystem::actionComponentStop(ActionComponent& action){
    action.state = ActionState::Stopped;
    action.stopTrigger = false;
    action.timecount = 0;

    action.onStop.call();
}

void ActionSystem::actionPause(Entity entity){
    ActionComponent& action = scene->getComponent<ActionComponent>(entity);

    actionComponentPause(action);
}

void ActionSystem::actionComponentPause(ActionComponent& action){
    action.state = ActionState::Paused;
    action.pauseTrigger = false;

    action.onPause.call();
}

void ActionSystem::actionUpdate(double dt, ActionComponent& action){
    action.timecount += dt * action.speed;

    action.onStep.call();
}

void ActionSystem::animationUpdate(double dt, Entity entity, ActionComponent& action, AnimationComponent& animcomp){
    int totalActionsPassed = 0;

    for (int i = 0; i < animcomp.actions.size(); i++){

        float timeDiff = action.timecount - animcomp.actions[i].startTime;

        Signature isignature = scene->getSignature(animcomp.actions[i].action);

        if (isignature.test(scene->getComponentType<ActionComponent>())){

            if (timeDiff >= 0) {
                //TODO: Support loop actions
                ActionComponent& iaction = scene->getComponent<ActionComponent>(animcomp.actions[i].action);
                if (iaction.state != ActionState::Running) {
                    actionStart(animcomp.actions[i].action);
                }

                iaction.timecount = timeDiff * iaction.speed;

                if (timeDiff > (animcomp.actions[i].duration / iaction.speed)) {
                    totalActionsPassed++;
                }
            }

        }

    }

    if (totalActionsPassed == animcomp.actions.size() || (animcomp.duration >= 0 && action.timecount >= (animcomp.duration / action.speed))) {
        if (!animcomp.loop) {
            actionStop(entity);
            //onFinish.call(object);
        }else{
            action.timecount = 0;
        }
    }
}

void ActionSystem::animationDestroy(AnimationComponent& animcomp){
    if (animcomp.ownedActions){
        for (int i = 0; i < animcomp.actions.size(); i++){
            scene->destroyEntity(animcomp.actions[i].action);
        }
    }
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
    spriteanim.frameTimeIndex = 0;
    spriteanim.spriteFrameCount = 0;

    setSpriteTextureRect(mesh, sprite, spriteanim);
}

void ActionSystem::spriteActionUpdate(double dt, Entity entity, ActionComponent& action, MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim){
    spriteanim.spriteFrameCount += dt * 1000;
    while (spriteanim.spriteFrameCount >= spriteanim.framesTime[spriteanim.frameTimeIndex]) {

        spriteanim.spriteFrameCount -= spriteanim.framesTime[spriteanim.frameTimeIndex];

        spriteanim.frameIndex++;
        spriteanim.frameTimeIndex++;

        if (spriteanim.frameIndex == spriteanim.framesSize - 1) {
            if (!spriteanim.loop) {
                actionStop(entity);
            }
        }

        if (spriteanim.frameIndex >= spriteanim.framesSize)
            spriteanim.frameIndex = 0;
        
        if (spriteanim.frameTimeIndex >= spriteanim.framesTimeSize)
            spriteanim.frameTimeIndex = 0;

        setSpriteTextureRect(mesh, sprite, spriteanim);
    }
}

void ActionSystem::timedActionStop(TimedActionComponent& timedaction){
    timedaction.time = 0;
    timedaction.value = 0;
}

void ActionSystem::timedActionUpdate(double dt, Entity entity, ActionComponent& action, TimedActionComponent& timedaction){
    if ((timedaction.time == 1) && !timedaction.loop){
        actionStop(entity);
        //onFinish.call(object);
    } else {
        float duration = timedaction.duration / action.speed;

        if (duration >= 0) {

            if (action.timecount >= duration){
                if (!timedaction.loop){
                    action.timecount = duration;
                }else{
                    action.timecount -= duration;
                }
            }

            timedaction.time = action.timecount / duration;
        }

        timedaction.value = timedaction.function.call(timedaction.time);
        //Log::debug("step time %f value %f \n", timedaction.time, timedaction.value);
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

void ActionSystem::colorActionMeshUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ColorActionComponent& coloraction, MeshComponent& mesh){
    Vector3 color = (coloraction.endColor - coloraction.startColor) * timedaction.value;
    if (coloraction.useSRGB){
        mesh.submeshes[0].material.baseColorFactor = Color::sRGBToLinear(coloraction.startColor + color);
    }else{
        mesh.submeshes[0].material.baseColorFactor = coloraction.startColor + color;
    }
}

void ActionSystem::colorActionUIUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ColorActionComponent& coloraction, UIComponent& uirender){
    Vector3 color = (coloraction.endColor - coloraction.startColor) * timedaction.value;
    if (coloraction.useSRGB){
        uirender.color = Color::sRGBToLinear(coloraction.startColor + color);
    }else{
        uirender.color = coloraction.startColor + color;
    }
}

void ActionSystem::alphaActionMeshUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, AlphaActionComponent& alphaaction, MeshComponent& mesh){
    float alpha = (alphaaction.endAlpha - alphaaction.startAlpha) * timedaction.value;

    mesh.submeshes[0].material.baseColorFactor.w = alphaaction.startAlpha + alpha;
}

void ActionSystem::alphaActionUIUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, AlphaActionComponent& alphaaction, UIComponent& uirender){
    float alpha = (alphaaction.endAlpha - alphaaction.startAlpha) * timedaction.value;

    uirender.color.w = alphaaction.startAlpha + alpha;
}

int ActionSystem::findUnusedParticle(PointParticlesComponent& particles, ParticlesAnimationComponent& partanim){

    for (int i=partanim.lastUsedParticle; i<particles.particles.size(); i++){
        if (particles.particles[i].life <= particles.particles[i].time){
            partanim.lastUsedParticle = i;
            return i;
        }
    }

    if (partanim.loop){
        for (int i=0; i<partanim.lastUsedParticle; i++){
            if (particles.particles[i].life <= particles.particles[i].time){
                partanim.lastUsedParticle = i;
                return i;
            }
        }
    }

    return -1;
}

int ActionSystem::findUnusedParticle(ParticlesComponent& particles){

    for (int i=particles.lastUsedParticle; i<particles.particles.size(); i++){
        if (particles.particles[i].life <= particles.particles[i].time){
            particles.lastUsedParticle = i;
            return i;
        }
    }

    if (particles.loop){
        for (int i=0; i<particles.lastUsedParticle; i++){
            if (particles.particles[i].life <= particles.particles[i].time){
                particles.lastUsedParticle = i;
                return i;
            }
        }
    }

    return -1;
}

float ActionSystem::getFloatInitializerValue(float& min, float& max){
    if (min != max) {
        return min + ((max - min) * (float) rand() / (float) RAND_MAX);
    }
    return max;
}

Vector3 ActionSystem::getVector3InitializerValue(Vector3& min, Vector3& max){
    if (min != max) {
        return Vector3( min.x + ((max.x - min.x) * (float) rand() / (float) RAND_MAX),
                        min.y + ((max.y - min.y) * (float) rand() / (float) RAND_MAX),
                        min.z + ((max.z - min.z) * (float) rand() / (float) RAND_MAX));
    }
    return max;
}

Rect ActionSystem::getSpriteInitializerValue(std::vector<int>& frames, PointParticlesComponent& particles){
    if (frames.size() > 0){
        int id = frames[int(frames.size()*rand()/(RAND_MAX + 1.0))];

        if (id >= 0 && id < MAX_SPRITE_FRAMES && particles.framesRect[id].active){
            return particles.framesRect[id].rect;
        }
    }

    return Rect(0,0,1,1);
}

Rect ActionSystem::getSpriteInitializerValue(std::vector<int>& frames, PointsComponent& points){
    if (frames.size() > 0){
        int id = frames[int(frames.size()*rand()/(RAND_MAX + 1.0))];

        if (id >= 0 && id < MAX_SPRITE_FRAMES && points.framesRect[id].active){
            return points.framesRect[id].rect;
        }
    }

    return Rect(0,0,1,1);
}

void ActionSystem::applyParticleInitializers(size_t idx, PointParticlesComponent& particles, ParticlesAnimationComponent& partanim){

    PointParticleLifeInitializer& lifeInit = partanim.lifeInitializer;
    particles.particles[idx].life = getFloatInitializerValue(lifeInit.minLife, lifeInit.maxLife);

    PointParticlePositionInitializer& posInit = partanim.positionInitializer;
    particles.particles[idx].position = getVector3InitializerValue(posInit.minPosition, posInit.maxPosition);

    PointParticleVelocityInitializer& velInit = partanim.velocityInitializer;
    particles.particles[idx].velocity = getVector3InitializerValue(velInit.minVelocity, velInit.maxVelocity);

    PointParticleAccelerationInitializer& accInit = partanim.accelerationInitializer;
    particles.particles[idx].acceleration = getVector3InitializerValue(accInit.minAcceleration, accInit.maxAcceleration);

    PointParticleColorInitializer& colInit = partanim.colorInitializer;
    particles.particles[idx].color = getVector3InitializerValue(colInit.minColor, colInit.maxColor);
    if (partanim.colorInitializer.useSRGB){
        particles.particles[idx].color = Color::sRGBToLinear(particles.particles[idx].color);
    }

    PointParticleAlphaInitializer& alpInit = partanim.alphaInitializer;
    particles.particles[idx].color.w = getFloatInitializerValue(alpInit.minAlpha, alpInit.maxAlpha);

    PointParticleSizeInitializer& sizeInit = partanim.sizeInitializer;
    particles.particles[idx].size = getFloatInitializerValue(sizeInit.minSize, sizeInit.maxSize);

    PointParticleSpriteInitializer& spriteInit = partanim.spriteInitializer;
    particles.particles[idx].textureRect = getSpriteInitializerValue(spriteInit.frames, particles);

    PointParticleRotationInitializer& rotInit = partanim.rotationInitializer;
    particles.particles[idx].rotation = Angle::defaultToRad(getFloatInitializerValue(rotInit.minRotation, rotInit.maxRotation));
}

void ActionSystem::applyParticleInitializers(size_t idx, ParticlesComponent& particles, InstancedMeshComponent& instmesh){
    ParticleLifeInitializer& lifeInit = particles.lifeInitializer;
    particles.particles[idx].life = getFloatInitializerValue(lifeInit.minLife, lifeInit.maxLife);

    ParticlePositionInitializer& posInit = particles.positionInitializer;
    instmesh.instances[idx].position = getVector3InitializerValue(posInit.minPosition, posInit.maxPosition);

    ParticleVelocityInitializer& velInit = particles.velocityInitializer;
    particles.particles[idx].velocity = getVector3InitializerValue(velInit.minVelocity, velInit.maxVelocity);

    ParticleAccelerationInitializer& accInit = particles.accelerationInitializer;
    particles.particles[idx].acceleration = getVector3InitializerValue(accInit.minAcceleration, accInit.maxAcceleration);
/*
    ParticleColorInitializer& colInit = particles.colorInitializer;
    particles.particles[idx].color = getVector3InitializerValue(colInit.minColor, colInit.maxColor);
    if (particles.colorInitializer.useSRGB){
        particles.particles[idx].color = Color::sRGBToLinear(particles.particles[idx].color);
    }

    ParticleAlphaInitializer& alpInit = particles.alphaInitializer;
    particles.particles[idx].color.w = getFloatInitializerValue(alpInit.minAlpha, alpInit.maxAlpha);

    ParticleSizeInitializer& sizeInit = particles.sizeInitializer;
    particles.particles[idx].size = getFloatInitializerValue(sizeInit.minSize, sizeInit.maxSize);

    ParticleSpriteInitializer& spriteInit = particles.spriteInitializer;
    particles.particles[idx].textureRect = getSpriteInitializerValue(spriteInit.frames, particles);

    ParticleRotationInitializer& rotInit = particles.rotationInitializer;
    particles.particles[idx].rotation = Angle::defaultToRad(getFloatInitializerValue(rotInit.minRotation, rotInit.maxRotation));
*/
}

void ActionSystem::applyParticleInitializers(size_t idx, ParticlesComponent& particles, PointsComponent& points){
    ParticleLifeInitializer& lifeInit = particles.lifeInitializer;
    particles.particles[idx].life = getFloatInitializerValue(lifeInit.minLife, lifeInit.maxLife);

    ParticlePositionInitializer& posInit = particles.positionInitializer;
    points.points[idx].position = getVector3InitializerValue(posInit.minPosition, posInit.maxPosition);

    ParticleVelocityInitializer& velInit = particles.velocityInitializer;
    particles.particles[idx].velocity = getVector3InitializerValue(velInit.minVelocity, velInit.maxVelocity);

    ParticleAccelerationInitializer& accInit = particles.accelerationInitializer;
    particles.particles[idx].acceleration = getVector3InitializerValue(accInit.minAcceleration, accInit.maxAcceleration);

    ParticleColorInitializer& colInit = particles.colorInitializer;
    points.points[idx].color = getVector3InitializerValue(colInit.minColor, colInit.maxColor);
    if (particles.colorInitializer.useSRGB){
        points.points[idx].color = Color::sRGBToLinear(points.points[idx].color);
    }

    ParticleAlphaInitializer& alpInit = particles.alphaInitializer;
    points.points[idx].color.w = getFloatInitializerValue(alpInit.minAlpha, alpInit.maxAlpha);

    ParticleSizeInitializer& sizeInit = particles.sizeInitializer;
    points.points[idx].size = getFloatInitializerValue(sizeInit.minSize, sizeInit.maxSize);

    ParticleSpriteInitializer& spriteInit = particles.spriteInitializer;
    points.points[idx].textureRect = getSpriteInitializerValue(spriteInit.frames, points);

    ParticleRotationInitializer& rotInit = particles.rotationInitializer;
    points.points[idx].rotation = Angle::defaultToRad(getFloatInitializerValue(rotInit.minRotation, rotInit.maxRotation));
}

float ActionSystem::getTimeFromParticleTime(float& time, float& fromTime, float& toTime){
    if ((fromTime != toTime) && (time >= fromTime) && (time <= toTime)) {
        return (time - fromTime) / (toTime - fromTime);
    }

    return -1;
}

float ActionSystem::getFloatModifierValue(float& value, float& fromValue, float& toValue){
    return fromValue + ((toValue - fromValue) * value);
}

Vector3 ActionSystem::getVector3ModifierValue(float& value, Vector3& fromValue, Vector3& toValue){
    return fromValue + ((toValue - fromValue) * value);
}

Rect ActionSystem::getSpriteModifierValue(float& value, std::vector<int>& frames, PointParticlesComponent& particles){
    if (frames.size() > 0){
        int id = frames[(int)(frames.size() * value)];

        if (id >= 0 && id < MAX_SPRITE_FRAMES && particles.framesRect[id].active){
            return particles.framesRect[id].rect;
        }
    }

    return Rect(0,0,1,1);
}

Rect ActionSystem::getSpriteModifierValue(float& value, std::vector<int>& frames, PointsComponent& points){
    if (frames.size() > 0){
        int id = frames[(int)(frames.size() * value)];

        if (id >= 0 && id < MAX_SPRITE_FRAMES && points.framesRect[id].active){
            return points.framesRect[id].rect;
        }
    }

    return Rect(0,0,1,1);
}

void ActionSystem::applyParticleModifiers(size_t idx, PointParticlesComponent& particles, ParticlesAnimationComponent& partanim){
    float particleTime = particles.particles[idx].time;
    float value;
    float time;

    PointParticlePositionModifier& posMod = partanim.positionModifier;
    time = getTimeFromParticleTime(particleTime, posMod.fromTime, posMod.toTime);
    value = posMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].position = getVector3ModifierValue(value, posMod.fromPosition, posMod.toPosition);
    }

    PointParticleVelocityModifier& velMod = partanim.velocityModifier;
    time = getTimeFromParticleTime(particleTime, velMod.fromTime, velMod.toTime);
    value = velMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].velocity = getVector3ModifierValue(value, velMod.fromVelocity, velMod.toVelocity);
    }

    PointParticleAccelerationModifier& accMod = partanim.accelerationModifier;
    time = getTimeFromParticleTime(particleTime, accMod.fromTime, accMod.toTime);
    value = accMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].acceleration = getVector3ModifierValue(value, accMod.fromAcceleration, accMod.toAcceleration);
    }

    PointParticleColorModifier& colMod = partanim.colorModifier;
    time = getTimeFromParticleTime(particleTime, colMod.fromTime, colMod.toTime);
    value = colMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].color = getVector3ModifierValue(value, colMod.fromColor, colMod.toColor);
        if (partanim.colorModifier.useSRGB){
            particles.particles[idx].color = Color::sRGBToLinear(particles.particles[idx].color);
        }
    }

    PointParticleAlphaModifier& alpMod = partanim.alphaModifier;
    time = getTimeFromParticleTime(particleTime, alpMod.fromTime, alpMod.toTime);
    value = alpMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].color.w = getFloatModifierValue(value, alpMod.fromAlpha, alpMod.toAlpha);
    }

    PointParticleSizeModifier& sizeMod = partanim.sizeModifier;
    time = getTimeFromParticleTime(particleTime, sizeMod.fromTime, sizeMod.toTime);
    value = sizeMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].size = getFloatModifierValue(value, sizeMod.fromSize, sizeMod.toSize);
    }

    PointParticleSpriteModifier& spriteMod = partanim.spriteModifier;
    time = getTimeFromParticleTime(particleTime, spriteMod.fromTime, spriteMod.toTime);
    value = spriteMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].textureRect = getSpriteModifierValue(value, spriteMod.frames, particles);
    }

    PointParticleRotationModifier& rotMod = partanim.rotationModifier;
    time = getTimeFromParticleTime(particleTime, rotMod.fromTime, rotMod.toTime);
    value = rotMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].rotation = Angle::defaultToRad(getFloatModifierValue(value, rotMod.fromRotation, rotMod.toRotation));
    }
}

void ActionSystem::applyParticleModifiers(size_t idx, ParticlesComponent& particles, InstancedMeshComponent& instmesh){
    float particleTime = particles.particles[idx].time;
    float value;
    float time;

    ParticlePositionModifier& posMod = particles.positionModifier;
    time = getTimeFromParticleTime(particleTime, posMod.fromTime, posMod.toTime);
    value = posMod.function.call(time);
    if (value >= 0 && value <= 1){
        instmesh.instances[idx].position = getVector3ModifierValue(value, posMod.fromPosition, posMod.toPosition);
    }

    ParticleVelocityModifier& velMod = particles.velocityModifier;
    time = getTimeFromParticleTime(particleTime, velMod.fromTime, velMod.toTime);
    value = velMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].velocity = getVector3ModifierValue(value, velMod.fromVelocity, velMod.toVelocity);
    }

    ParticleAccelerationModifier& accMod = particles.accelerationModifier;
    time = getTimeFromParticleTime(particleTime, accMod.fromTime, accMod.toTime);
    value = accMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].acceleration = getVector3ModifierValue(value, accMod.fromAcceleration, accMod.toAcceleration);
    }
    /*
    ParticleColorModifier& colMod = partanim.colorModifier;
    time = getTimeFromParticleTime(particleTime, colMod.fromTime, colMod.toTime);
    value = colMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].color = getVector3ModifierValue(value, colMod.fromColor, colMod.toColor);
        if (partanim.colorModifier.useSRGB){
            particles.particles[idx].color = Color::sRGBToLinear(particles.particles[idx].color);
        }
    }

    ParticleAlphaModifier& alpMod = partanim.alphaModifier;
    time = getTimeFromParticleTime(particleTime, alpMod.fromTime, alpMod.toTime);
    value = alpMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].color.w = getFloatModifierValue(value, alpMod.fromAlpha, alpMod.toAlpha);
    }

    ParticleSizeModifier& sizeMod = partanim.sizeModifier;
    time = getTimeFromParticleTime(particleTime, sizeMod.fromTime, sizeMod.toTime);
    value = sizeMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].size = getFloatModifierValue(value, sizeMod.fromSize, sizeMod.toSize);
    }

    ParticleSpriteModifier& spriteMod = partanim.spriteModifier;
    time = getTimeFromParticleTime(particleTime, spriteMod.fromTime, spriteMod.toTime);
    value = spriteMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].textureRect = getSpriteModifierValue(value, spriteMod.frames, particles);
    }

    ParticleRotationModifier& rotMod = partanim.rotationModifier;
    time = getTimeFromParticleTime(particleTime, rotMod.fromTime, rotMod.toTime);
    value = rotMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].rotation = Angle::defaultToRad(getFloatModifierValue(value, rotMod.fromRotation, rotMod.toRotation));
    }
    */
}

void ActionSystem::applyParticleModifiers(size_t idx, ParticlesComponent& particles, PointsComponent& points){
    float particleTime = particles.particles[idx].time;
    float value;
    float time;

    ParticlePositionModifier& posMod = particles.positionModifier;
    time = getTimeFromParticleTime(particleTime, posMod.fromTime, posMod.toTime);
    value = posMod.function.call(time);
    if (value >= 0 && value <= 1){
        points.points[idx].position = getVector3ModifierValue(value, posMod.fromPosition, posMod.toPosition);
    }

    ParticleVelocityModifier& velMod = particles.velocityModifier;
    time = getTimeFromParticleTime(particleTime, velMod.fromTime, velMod.toTime);
    value = velMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].velocity = getVector3ModifierValue(value, velMod.fromVelocity, velMod.toVelocity);
    }

    ParticleAccelerationModifier& accMod = particles.accelerationModifier;
    time = getTimeFromParticleTime(particleTime, accMod.fromTime, accMod.toTime);
    value = accMod.function.call(time);
    if (value >= 0 && value <= 1){
        particles.particles[idx].acceleration = getVector3ModifierValue(value, accMod.fromAcceleration, accMod.toAcceleration);
    }

    ParticleColorModifier& colMod = particles.colorModifier;
    time = getTimeFromParticleTime(particleTime, colMod.fromTime, colMod.toTime);
    value = colMod.function.call(time);
    if (value >= 0 && value <= 1){
        points.points[idx].color = getVector3ModifierValue(value, colMod.fromColor, colMod.toColor);
        if (particles.colorModifier.useSRGB){
            points.points[idx].color = Color::sRGBToLinear(points.points[idx].color);
        }
    }

    ParticleAlphaModifier& alpMod = particles.alphaModifier;
    time = getTimeFromParticleTime(particleTime, alpMod.fromTime, alpMod.toTime);
    value = alpMod.function.call(time);
    if (value >= 0 && value <= 1){
        points.points[idx].color.w = getFloatModifierValue(value, alpMod.fromAlpha, alpMod.toAlpha);
    }

    ParticleSizeModifier& sizeMod = particles.sizeModifier;
    time = getTimeFromParticleTime(particleTime, sizeMod.fromTime, sizeMod.toTime);
    value = sizeMod.function.call(time);
    if (value >= 0 && value <= 1){
        points.points[idx].size = getFloatModifierValue(value, sizeMod.fromSize, sizeMod.toSize);
    }

    ParticleSpriteModifier& spriteMod = particles.spriteModifier;
    time = getTimeFromParticleTime(particleTime, spriteMod.fromTime, spriteMod.toTime);
    value = spriteMod.function.call(time);
    if (value >= 0 && value <= 1){
        points.points[idx].textureRect = getSpriteModifierValue(value, spriteMod.frames, points);
    }

    ParticleRotationModifier& rotMod = particles.rotationModifier;
    time = getTimeFromParticleTime(particleTime, rotMod.fromTime, rotMod.toTime);
    value = rotMod.function.call(time);
    if (value >= 0 && value <= 1){
        points.points[idx].rotation = Angle::defaultToRad(getFloatModifierValue(value, rotMod.fromRotation, rotMod.toRotation));
    }
}

void ActionSystem::particleActionStart(ParticlesAnimationComponent& partanim, PointParticlesComponent& particles){
    if (partanim.sizeInitializer.minSize == 0 && partanim.sizeInitializer.maxSize == 0){
        float size = std::max(particles.texture.getWidth(), particles.texture.getHeight());
        partanim.sizeInitializer.minSize = size;
        partanim.sizeInitializer.maxSize = size;
    }

    // Creating particles
    particles.particles.clear();
    for (int i = 0; i < particles.maxParticles; i++){
        particles.particles.push_back({});
        particles.particles.back().life = 0;
        particles.particles.back().time = 0;
    }

    partanim.emitter = true;
    partanim.newParticlesCount = 0;
    partanim.lastUsedParticle = 0;
}

void ActionSystem::particleActionStart(ParticlesComponent& particles, InstancedMeshComponent& instmesh){
    //if (particles.sizeInitializer.minSize == 0 && particles.sizeInitializer.maxSize == 0){
        //float size = std::max(particles.texture.getWidth(), particles.texture.getHeight());
        //partanim.sizeInitializer.minSize = size;
        //partanim.sizeInitializer.maxSize = size;
    //}

    // Creating particles
    particles.particles.clear();
    instmesh.instances.clear();
    for (int i = 0; i < particles.maxParticles; i++){
        particles.particles.push_back({});
        particles.particles.back().life = 0;
        particles.particles.back().time = 0;

        instmesh.instances.push_back({});

        instmesh.needUpdateInstances = true;
    }

    particles.emitter = true;
    particles.newParticlesCount = 0;
    particles.lastUsedParticle = 0;
}

void ActionSystem::particleActionStart(ParticlesComponent& particles, PointsComponent& points){
    if (particles.sizeInitializer.minSize == 0 && particles.sizeInitializer.maxSize == 0){
        float size = std::max(points.texture.getWidth(), points.texture.getHeight());
        particles.sizeInitializer.minSize = size;
        particles.sizeInitializer.maxSize = size;
    }

    // Creating particles
    particles.particles.clear();
    points.points.clear();
    for (int i = 0; i < particles.maxParticles; i++){
        particles.particles.push_back({});
        particles.particles.back().life = 0;
        particles.particles.back().time = 0;

        points.points.push_back({});

        points.needUpdate = true;
    }

    particles.emitter = true;
    particles.newParticlesCount = 0;
    particles.lastUsedParticle = 0;
}

void ActionSystem::particlesActionUpdate(double dt, Entity entity, ActionComponent& action, ParticlesAnimationComponent& partanim, PointParticlesComponent& particles){
    if (partanim.emitter){
        partanim.newParticlesCount += dt * partanim.rate;

        int newparticles = (int)partanim.newParticlesCount;
        partanim.newParticlesCount -= newparticles;
        if (newparticles > partanim.maxPerUpdate)
            newparticles = partanim.maxPerUpdate;

        for(int i=0; i<newparticles; i++){
            int particleIndex = findUnusedParticle(particles, partanim);

            if (particleIndex >= 0){
                particles.particles[particleIndex].time = 0;
                applyParticleInitializers(particleIndex, particles, partanim);
                particles.needUpdate = true;
            }else{
                if (!partanim.loop)
                    partanim.emitter = false;
                break;
            }
        }
    }

    bool existParticles = false;
    for(int i=0; i<particles.particles.size(); i++){

        float life = particles.particles[i].life;
        float time = particles.particles[i].time;

        if(life > time){

            applyParticleModifiers(i, particles, partanim);

            Vector3 velocity = particles.particles[i].velocity;
            Vector3 position = particles.particles[i].position;
            Vector3 acceleration = particles.particles[i].acceleration;

            velocity += acceleration * dt * 0.5f;
            position += velocity * dt;
            time += dt;

            particles.particles[i].time = time;
            particles.particles[i].velocity = velocity;
            particles.particles[i].position = position;

            existParticles = true;

            particles.needUpdate = true;

            //printf("1.Particle %i life %f time %f position %f %f %f\n", i, life, time, position.x, position.y, position.z);
        }
    }

    if (!existParticles && !partanim.emitter){
        actionStop(entity);
        //onFinish.call(object);
    }
}

void ActionSystem::particlesActionUpdate(double dt, Entity entity, ActionComponent& action, ParticlesComponent& particles, InstancedMeshComponent& instmesh){
    if (particles.emitter){
        particles.newParticlesCount += dt * particles.rate;

        int newparticles = (int)particles.newParticlesCount;
        particles.newParticlesCount -= newparticles;
        if (newparticles > particles.maxPerUpdate)
            newparticles = particles.maxPerUpdate;

        for(int i=0; i<newparticles; i++){
            int particleIndex = findUnusedParticle(particles);

            if (particleIndex >= 0){
                particles.particles[particleIndex].time = 0;
                applyParticleInitializers(particleIndex, particles, instmesh);
                instmesh.needUpdateInstances = true;
            }else{
                if (!particles.loop)
                    particles.emitter = false;
                break;
            }
        }
    }

    bool existParticles = false;
    for(int i=0; i<particles.particles.size(); i++){

        float life = particles.particles[i].life;
        float time = particles.particles[i].time;

        if(life > time){

            applyParticleModifiers(i, particles, instmesh);

            Vector3 velocity = particles.particles[i].velocity;
            Vector3 position = instmesh.instances[i].position;
            Vector3 acceleration = particles.particles[i].acceleration;

            velocity += acceleration * dt * 0.5f;
            position += velocity * dt;
            time += dt;

            particles.particles[i].time = time;
            particles.particles[i].velocity = velocity;
            instmesh.instances[i].position = position;

            existParticles = true;

            instmesh.instances[i].visible = true;
            instmesh.needUpdateInstances = true;

            //printf("1.Particle %i life %f time %f position %f %f %f\n", i, life, time, position.x, position.y, position.z);
        }else{
            instmesh.instances[i].visible = false;
        }
    }

    if (!existParticles && !particles.emitter){
        actionStop(entity);
        //onFinish.call(object);
    }
}

void ActionSystem::particlesActionUpdate(double dt, Entity entity, ActionComponent& action, ParticlesComponent& particles, PointsComponent& points){
    if (particles.emitter){
        particles.newParticlesCount += dt * particles.rate;

        int newparticles = (int)particles.newParticlesCount;
        particles.newParticlesCount -= newparticles;
        if (newparticles > particles.maxPerUpdate)
            newparticles = particles.maxPerUpdate;

        for(int i=0; i<newparticles; i++){
            int particleIndex = findUnusedParticle(particles);

            if (particleIndex >= 0){
                particles.particles[particleIndex].time = 0;
                applyParticleInitializers(particleIndex, particles, points);
                points.needUpdate = true;
            }else{
                if (!particles.loop)
                    particles.emitter = false;
                break;
            }
        }
    }

    bool existParticles = false;
    for(int i=0; i<particles.particles.size(); i++){

        float life = particles.particles[i].life;
        float time = particles.particles[i].time;

        if(life > time){
            applyParticleModifiers(i, particles, points);

            Vector3 velocity = particles.particles[i].velocity;
            Vector3 position = points.points[i].position;
            Vector3 acceleration = particles.particles[i].acceleration;

            velocity += acceleration * dt * 0.5f;
            position += velocity * dt;
            time += dt;

            particles.particles[i].time = time;
            particles.particles[i].velocity = velocity;
            points.points[i].position = position;

            existParticles = true;

            points.points[i].visible = true;
            points.needUpdate = true;

            //printf("1.Particle %i life %f time %f position %f %f %f\n", i, life, time, position.x, position.y, position.z);
        }else{
            points.points[i].visible = false;
        }
    }

    if (!existParticles && !particles.emitter){
        actionStop(entity);
        //onFinish.call(object);
    }
}

void ActionSystem::keyframeUpdate(double dt, ActionComponent& action, KeyframeTracksComponent& keyframe){
    if (keyframe.times.size() == 0)
        return;

    float currentTime = action.timecount;

    keyframe.interpolation = 0;

    keyframe.index = 0;
    while (keyframe.index < (keyframe.times.size()-1) && keyframe.times[keyframe.index] < currentTime){
        keyframe.index++;
    }

    float previousTime = 0;
    float nextTime = keyframe.times[keyframe.index];

    if (keyframe.index > 0){
        previousTime = keyframe.times[keyframe.index-1];
    }

    if (nextTime > previousTime){
        keyframe.interpolation = (currentTime - previousTime) / (nextTime - previousTime);
    }

    if (keyframe.interpolation > 1){
        keyframe.interpolation = 1;
    }
}

void ActionSystem::translateTracksUpdate(KeyframeTracksComponent& keyframe, TranslateTracksComponent& translatetracks, Transform& transform){
    Vector3 previousTranslation = translatetracks.values[0];
    if (keyframe.index > 0){
        previousTranslation = translatetracks.values[keyframe.index-1];
    }

    transform.position = previousTranslation + keyframe.interpolation * (translatetracks.values[keyframe.index] - previousTranslation);
    transform.needUpdate = true;
}

void ActionSystem::scaleTracksUpdate(KeyframeTracksComponent& keyframe, ScaleTracksComponent& scaletracks, Transform& transform){
    Vector3 previousScale = scaletracks.values[0];
    if (keyframe.index > 0){
        previousScale = scaletracks.values[keyframe.index-1];
    }

    transform.scale = previousScale + keyframe.interpolation * (scaletracks.values[keyframe.index] - previousScale);
    transform.needUpdate = true;
}

void ActionSystem::rotateTracksUpdate(KeyframeTracksComponent& keyframe, RotateTracksComponent& rotatetracks, Transform& transform){
    Quaternion previousRotation = rotatetracks.values[0];
    if (keyframe.index > 0){
        previousRotation = rotatetracks.values[keyframe.index-1];
    }

    transform.rotation = transform.rotation.slerp(keyframe.interpolation, previousRotation, rotatetracks.values[keyframe.index]);
    transform.needUpdate = true;
}

void ActionSystem::morphTracksUpdate(KeyframeTracksComponent& keyframe, MorphTracksComponent& morpthtracks, MeshComponent& mesh){
    std::vector<float> previousMorph = morpthtracks.values[0];
    if (keyframe.index > 0){
        previousMorph = morpthtracks.values[keyframe.index-1];
    }

    if ((keyframe.index == 0) || (morpthtracks.values[keyframe.index].size() == morpthtracks.values[keyframe.index-1].size())) {
        for (int morphIndex = 0; morphIndex < morpthtracks.values[keyframe.index].size(); morphIndex++) {
            mesh.morphWeights[morphIndex] = previousMorph[morphIndex] + keyframe.interpolation * (morpthtracks.values[keyframe.index][morphIndex] - previousMorph[morphIndex]);
        }
    }else{
        Log::error("MorphTrack of index %i is different size than index %i", keyframe.index, keyframe.index-1);
    }
}

void ActionSystem::load(){

}

void ActionSystem::destroy(){

    auto actions = scene->getComponentArray<ActionComponent>();
    for (int i = 0; i < actions->size(); i++) {
        ActionComponent &action = actions->getComponentFromIndex(i);
        Entity entity = actions->getEntity(i);
        Signature signature = scene->getSignature(entity);

        action.state = ActionState::Stopped;
        action.timecount = 0;
        action.startTrigger = false;
        action.stopTrigger = false;
        action.pauseTrigger = false;

        if (signature.test(scene->getComponentType<TimedActionComponent>())) {
            TimedActionComponent &timedaction = scene->getComponent<TimedActionComponent>(entity);
            timedaction.time = 0;
            timedaction.value = 0;
        }
    }
}

void ActionSystem::draw(){

}

void ActionSystem::actionStateChange(Entity entity, ActionComponent& action){
    // Action start
    if (action.startTrigger == true && (action.state == ActionState::Stopped || action.state == ActionState::Paused)){
        actionStart(entity);
    }

    // Action stop
    if (action.stopTrigger == true && (action.state == ActionState::Running || action.state == ActionState::Paused)){
        actionStop(entity);
    }

    // Action pause
    if (action.pauseTrigger == true && action.state == ActionState::Running){
        actionPause(entity);
    }
}

void ActionSystem::update(double dt){

    //Animations actions
    auto animations = scene->getComponentArray<AnimationComponent>();
    for (int i = 0; i < animations->size(); i++){
        AnimationComponent& animcomp = animations->getComponentFromIndex(i);

        Entity entity = animations->getEntity(i);
        Signature signature = scene->getSignature(entity);

        if (signature.test(scene->getComponentType<ActionComponent>())){
            ActionComponent& action = scene->getComponent<ActionComponent>(entity);

            actionStateChange(entity, action);

            if (action.state == ActionState::Running){
                animationUpdate(dt, entity, action, animcomp);
            }
        }
    }

    //All actions
    auto actions = scene->getComponentArray<ActionComponent>();
    for (int i = 0; i < actions->size(); i++){
		ActionComponent& action = actions->getComponentFromIndex(i);

        actionStateChange(actions->getEntity(i), action);

        // Action update
        if (action.state == ActionState::Running){

            Entity entity = actions->getEntity(i);
            Signature signature = scene->getSignature(entity);
            Signature targetSignature;
            if (action.target != NULL_ENTITY){
                targetSignature = scene->getSignature(action.target);
            }

            //Sprite animation
            if (signature.test(scene->getComponentType<SpriteAnimationComponent>())){
                SpriteAnimationComponent& spriteanim = scene->getComponent<SpriteAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<SpriteComponent>()) && targetSignature.test(scene->getComponentType<MeshComponent>())){
                    SpriteComponent& sprite = scene->getComponent<SpriteComponent>(action.target);
                    MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                    spriteActionUpdate(dt, entity, action, mesh, sprite, spriteanim);
                    if (action.state != ActionState::Running) continue;
                }
            }

            //Particles
            if (signature.test(scene->getComponentType<ParticlesComponent>())){
                ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(entity);

                if (targetSignature.test(scene->getComponentType<InstancedMeshComponent>())){
                    InstancedMeshComponent& instmesh = scene->getComponent<InstancedMeshComponent>(action.target);

                    particlesActionUpdate(dt, entity, action, particles, instmesh);
                    if (action.state != ActionState::Running) continue;
                }
            }
            if (signature.test(scene->getComponentType<ParticlesAnimationComponent>())){
                ParticlesAnimationComponent& partanim = scene->getComponent<ParticlesAnimationComponent>(entity);

                if (targetSignature.test(scene->getComponentType<PointParticlesComponent>())){
                    PointParticlesComponent& particles = scene->getComponent<PointParticlesComponent>(action.target);

                    particlesActionUpdate(dt, entity, action, partanim, particles);
                    if (action.state != ActionState::Running) continue;
                }
            }

            //keyframe animation
            if (signature.test(scene->getComponentType<KeyframeTracksComponent>())){
                KeyframeTracksComponent& keyframe = scene->getComponent<KeyframeTracksComponent>(entity);

                keyframeUpdate(dt, action, keyframe);
                if (action.state != ActionState::Running) continue;

                if (signature.test(scene->getComponentType<TranslateTracksComponent>())){
                    TranslateTracksComponent& translatetracks = scene->getComponent<TranslateTracksComponent>(entity);

                    if (targetSignature.test(scene->getComponentType<Transform>())){
                        Transform& transform = scene->getComponent<Transform>(action.target);

                        translateTracksUpdate(keyframe, translatetracks, transform);
                    }
                }

                if (signature.test(scene->getComponentType<RotateTracksComponent>())){
                    RotateTracksComponent& rotatetracks = scene->getComponent<RotateTracksComponent>(entity);

                    if (targetSignature.test(scene->getComponentType<Transform>())){
                        Transform& transform = scene->getComponent<Transform>(action.target);

                        rotateTracksUpdate(keyframe, rotatetracks, transform);
                    }
                }

                if (signature.test(scene->getComponentType<ScaleTracksComponent>())){
                    ScaleTracksComponent& scaletracks = scene->getComponent<ScaleTracksComponent>(entity);

                    if (targetSignature.test(scene->getComponentType<Transform>())){
                        Transform& transform = scene->getComponent<Transform>(action.target);

                        scaleTracksUpdate(keyframe, scaletracks, transform);
                    }
                }

                if (signature.test(scene->getComponentType<MorphTracksComponent>())){
                    MorphTracksComponent& morpthtracks = scene->getComponent<MorphTracksComponent>(entity);

                    if (targetSignature.test(scene->getComponentType<MeshComponent>())){
                        MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                        morphTracksUpdate(keyframe, morpthtracks, mesh);
                    }
                }
            }


            if (signature.test(scene->getComponentType<TimedActionComponent>())){
                TimedActionComponent& timedaction = scene->getComponent<TimedActionComponent>(entity);

                timedActionUpdate(dt, entity, action, timedaction);
                if (action.state != ActionState::Running) continue;

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

                        colorActionMeshUpdate(dt, action, timedaction, coloraction, mesh);
                    }
                    if (targetSignature.test(scene->getComponentType<UIComponent>())){
                        UIComponent& uirender = scene->getComponent<UIComponent>(action.target);

                        colorActionUIUpdate(dt, action, timedaction, coloraction, uirender);
                    }
                }

                //Alpha animation
                if (signature.test(scene->getComponentType<AlphaActionComponent>())){
                    AlphaActionComponent& alphaaction = scene->getComponent<AlphaActionComponent>(entity);

                    if (targetSignature.test(scene->getComponentType<MeshComponent>())){
                        MeshComponent& mesh = scene->getComponent<MeshComponent>(action.target);

                        alphaActionMeshUpdate(dt, action, timedaction, alphaaction, mesh);
                    }
                    if (targetSignature.test(scene->getComponentType<UIComponent>())){
                        UIComponent& uirender = scene->getComponent<UIComponent>(action.target);

                        alphaActionUIUpdate(dt, action, timedaction, alphaaction, uirender);
                    }
                }

            }

            actionUpdate(dt, action);
        }

	}
}

void ActionSystem::entityDestroyed(Entity entity){
	Signature signature = scene->getSignature(entity);

	if (signature.test(scene->getComponentType<AnimationComponent>())){
		animationDestroy(scene->getComponent<AnimationComponent>(entity));
	}
}