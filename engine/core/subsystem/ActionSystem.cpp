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

void ActionSystem::timedActionStop(TimedActionComponent& timedaction){
    timedaction.timecount = 0;
    timedaction.time = 0;
    timedaction.value = 0;
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

int ActionSystem::findUnusedParticle(ParticlesComponent& particles, ParticlesAnimationComponent& partanim){

    for (int i=partanim.lastUsedParticle; i<particles.particles.size(); i++){
        if (particles.particles[i].life <= 0){
            partanim.lastUsedParticle = i;
            return i;
        }
    }

    for (int i=0; i<partanim.lastUsedParticle; i++){
        if (particles.particles[i].life <= 0){
            partanim.lastUsedParticle = i;
            return i;
        }
    }

    return -1;
}

void ActionSystem::applyParticleInitializers(size_t idx, ParticlesComponent& particles, ParticlesAnimationComponent& partanim){

    ParticleVelocityInitializer& velInit = partanim.velocityInitializer;
    Vector3 velocity;

    if (velInit.minVelocity != velInit.maxVelocity) {
        velocity = Vector3(velInit.minVelocity.x + ((velInit.maxVelocity.x - velInit.minVelocity.x) * (float) rand() / (float) RAND_MAX),
                           velInit.minVelocity.y + ((velInit.maxVelocity.y - velInit.minVelocity.y) * (float) rand() / (float) RAND_MAX),
                           velInit.minVelocity.z + ((velInit.maxVelocity.z - velInit.minVelocity.z) * (float) rand() / (float) RAND_MAX));
    }else{
        velocity = velInit.maxVelocity;
    }

    particles.particles[idx].velocity = velocity;
    //printf("Particle %i velocity %f %f %f", (int)i, velocity.x, velocity.y, velocity.z);

}

void ActionSystem::applyParticleModifiers(size_t idx, ParticlesComponent& particles, ParticlesAnimationComponent& partanim){

    float life = particles.particles[idx].life;

    ParticleVelocityModifier& velMod = partanim.velocityModifier;

    float time;
    if ((velMod.fromLife != velMod.toLife) && (life <= velMod.fromLife) && (life >= velMod.toLife)) {
        time = (life - velMod.fromLife) / (velMod.toLife - velMod.fromLife);
    }else{
        time = -1;
    }

    float value = time; //TODO: EASE function

    if (value >= 0 && value <= 1){
        Vector3 velocity = velMod.fromVelocity + ((velMod.toVelocity - velMod.fromVelocity) * value);
        particles.particles[idx].velocity = velocity;
    }

}

void ActionSystem::particleActionStart(ParticlesAnimationComponent& partanim){
    partanim.emitter = true;
}

void ActionSystem::particlesActionUpdate(double dt, Entity entity, ActionComponent& action, ParticlesAnimationComponent& partanim, ParticlesComponent& particles){
    if (partanim.emitter){
        partanim.newParticlesCount += dt * partanim.rate;

        int newparticles = (int)partanim.newParticlesCount;
        partanim.newParticlesCount -= newparticles;
        if (newparticles > partanim.maxPerUpdate)
            newparticles = partanim.maxPerUpdate;

        for(int i=0; i<newparticles; i++){
            int particleIndex = findUnusedParticle(particles, partanim);

            if (particleIndex >= 0){

                particles.particles[particleIndex].life = 10;
                particles.particles[particleIndex].position = Vector3(0,0,0);
                particles.particles[particleIndex].velocity = Vector3(0,0,0);
                particles.particles[particleIndex].acceleration = Vector3(0,0,0);
                particles.particles[particleIndex].color = Vector4(1,1,1,1);
                particles.particles[particleIndex].size = 20;
                //particles->setParticleSprite(particleIndex, -1);

                applyParticleInitializers(particleIndex, particles, partanim);

                particles.needUpdate = true;
            }

        }
    }

    bool existParticles = false;
    for(int i=0; i<particles.particles.size(); i++){

        float life = particles.particles[i].life;

        if(life > 0.0f){

            applyParticleModifiers(i, particles, partanim);

            Vector3 velocity = particles.particles[i].velocity;
            Vector3 position = particles.particles[i].position;
            Vector3 acceleration = particles.particles[i].acceleration;

            velocity += acceleration * dt * 0.5f;
            position += velocity * dt;
            life -= dt;

            particles.particles[i].life = life;
            particles.particles[i].velocity = velocity;
            particles.particles[i].position = position;

            existParticles = true;

            particles.needUpdate = true;

            //printf("1.Particle %i life %f position %f %f %f\n", i, life, position.x, position.y, position.z);
        }
    }

    if (!existParticles && !partanim.emitter){
        action.stopTrigger = true;
        //onFinish.call(object);
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

            if (signature.test(scene->getComponentType<ParticlesAnimationComponent>())){
                ParticlesAnimationComponent& partanim = scene->getComponent<ParticlesAnimationComponent>(entity);
                if (targetSignature.test(scene->getComponentType<ParticlesComponent>()) ){

                    particleActionStart(partanim);

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

            if (signature.test(scene->getComponentType<TimedActionComponent>())){
                TimedActionComponent& timedaction = scene->getComponent<TimedActionComponent>(entity);

                timedActionStop(timedaction);
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

            //Particles
            if (signature.test(scene->getComponentType<ParticlesAnimationComponent>())){
                ParticlesAnimationComponent& partanim = scene->getComponent<ParticlesAnimationComponent>(entity);

                if (targetSignature.test(scene->getComponentType<ParticlesComponent>())){
                    ParticlesComponent& particles = scene->getComponent<ParticlesComponent>(action.target);

                    particlesActionUpdate(dt, entity, action, partanim, particles);
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