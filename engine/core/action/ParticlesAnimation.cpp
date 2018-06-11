#include "ParticlesAnimation.h"
#include "Particles.h"
#include "Log.h"

#include <stdlib.h> 

using namespace Supernova;


ParticlesAnimation::ParticlesAnimation(): Action(){
    newParticlesCount = 0;
    emitter = true;

    initOwned = true;
    modOwned = true;
}

ParticlesAnimation::~ParticlesAnimation(){
    if (initOwned)
        deleteInits();

    if (modOwned)
        deleteMods();
    
}

void ParticlesAnimation::addInit(ParticleInit* particleInit){
    particlesInit.push_back(particleInit);
}

void ParticlesAnimation::removeInit(ParticleInit* particleInit){
    std::vector<ParticleInit*>::iterator i = std::remove(particlesInit.begin(), particlesInit.end(), particleInit);
    particlesInit.erase(i,particlesInit.end());
}

void ParticlesAnimation::deleteInits(){
    for (std::vector<ParticleInit*>::iterator it = particlesInit.begin() ; it != particlesInit.end(); ++it) {
        delete (*it);
    }
    particlesInit.clear();
}

void ParticlesAnimation::setInitOwned(bool initOwned){
    this->initOwned = initOwned;
}

void ParticlesAnimation::addMod(ParticleMod* particleMod){
    particlesMod.push_back(particleMod);
}

void ParticlesAnimation::removeMod(ParticleMod* particleMod){
    std::vector<ParticleMod*>::iterator i = std::remove(particlesMod.begin(), particlesMod.end(), particleMod);
    particlesMod.erase(i,particlesMod.end());
}

void ParticlesAnimation::deleteMods(){
    for (std::vector<ParticleMod*>::iterator it = particlesMod.begin() ; it != particlesMod.end(); ++it) {
        delete (*it);
    }
    particlesMod.clear();
}

void ParticlesAnimation::setModOwned(bool modOwned){
    this->modOwned = modOwned;
}

bool ParticlesAnimation::run(){
    if (!Action::run())
        return false;
    
    if (Particles* particles = dynamic_cast<Particles*>(object)) {
        emitter = true;
    }else{
        Log::Error("Object in ParticlesAnimation must be a Particles type");
        stop();
    }
    
    return true;
}

bool ParticlesAnimation::pause(){
    return Action::pause();
}

bool ParticlesAnimation::stop(){
    if (!Action::stop())
        return false;
    
    if (Particles* particles = dynamic_cast<Particles*>(object)){
        for (int i = 0; i < particles->getMaxParticles(); i++) {
            particles->setParticleLife(i, -1);
        }
        
        particles->updateParticles();
    }
    
    newParticlesCount = 0;
    emitter = true;
    
    return true;
}

void ParticlesAnimation::startEmitter(){
    if (!running) {
        emitter = true;
    }else{
        run();
    }
}

void ParticlesAnimation::stopEmitter(){
    emitter = false;
}

bool ParticlesAnimation::isEmitting(){
    return emitter;
}

bool ParticlesAnimation::step(){
    if (!Action::step())
        return false;

    if (Particles* particles = dynamic_cast<Particles*>(object)){
        
        float delta = (float)steptime / 1000;

        if (emitter){
            newParticlesCount += delta * particles->getMinRate();

            int newparticles = (int)newParticlesCount;
            newParticlesCount -= newparticles;

            if (newparticles > particles->getMaxRate())
                newparticles = particles->getMaxRate();

            for(int i=0; i<newparticles; i++){
                int particleIndex = particles->findUnusedParticle();
                
                if (particleIndex >= 0){

                    particles->setParticleLife(particleIndex, 10);
                    particles->setParticlePosition(particleIndex, Vector3(0,0,0));
                    particles->setParticleVelocity(particleIndex, Vector3(0,0,0));
                    particles->setParticleAcceleration(particleIndex, Vector3(0,0,0));
                    particles->setParticleColor(particleIndex, Vector4(1,1,1,1));
                    particles->setParticleSize(particleIndex, 1);
                    particles->setParticleSprite(particleIndex, -1);

                    for (int init=0; init < particlesInit.size(); init++){
                        particlesInit[init]->execute(particles, particleIndex);
                    }
                    
                }
            }
        }

        bool existParticles = false;
        for(int i=0; i<particles->getMaxParticles(); i++){
            
            float life = particles->getParticleLife(i);
            
            if(life > 0.0f){

                for (int mod=0; mod < particlesMod.size(); mod++){
                    particlesMod[mod]->execute(particles, i, life);
                }
                
                Vector3 velocity = particles->getParticleVelocity(i);
                Vector3 position = particles->getParticlePosition(i);
                Vector3 acceleration = particles->getParticleAcceleration(i);
                
                velocity += acceleration * (float)delta * 0.5f;
                position += velocity * (float)delta;
                life -= delta;

                particles->setParticleLife(i, life);
                particles->setParticleVelocity(i, velocity);
                particles->setParticlePosition(i, position);
                
                existParticles = true;
                
                //printf("1.Particle %i life %f position %f %f %f\n", i, life, position.x, position.y, position.z);
            }
        }
        
        if (!existParticles && !emitter){
            stop();
            onFinish.call(object);
        }

        particles->updateParticles();

    }

    return true;
}
