#include "ParticlesAnimation.h"
#include "Particles.h"
#include "platform/Log.h"

#include <stdlib.h> 

using namespace Supernova;


ParticlesAnimation::ParticlesAnimation(): Action(-1, true){
    reset();
}

ParticlesAnimation::~ParticlesAnimation(){
    
}

void ParticlesAnimation::addInit(ParticleAnimationInit* particleInit){
    particlesInit.push_back(particleInit);
}

void ParticlesAnimation::removeInit(ParticleAnimationInit* particleInit){
    std::vector<ParticleAnimationInit*>::iterator i = std::remove(particlesInit.begin(), particlesInit.end(), particleInit);
    particlesInit.erase(i,particlesInit.end());
}

void ParticlesAnimation::play(){
    Action::play();
    
    if (Particles* particles = dynamic_cast<Particles*>(object)) {
        for (int i = 0; i < particles->getMaxParticles(); i++) {
            particles->setParticleLife(i, 0);
        }
    }else{
        Log::Error(LOG_TAG, "Object in ParticlesAnimation must be a Particles type");
        stop();
    }
}

void ParticlesAnimation::stop(){
    Action::stop();
}

void ParticlesAnimation::reset(){
    Action::reset();

    newParticlesCount = 0;
}

void ParticlesAnimation::step(){
    Action::step();

    if (Particles* particles = dynamic_cast<Particles*>(object)){
        
        float delta = (float)steptime / 1000;

        newParticlesCount += delta * particles->getMinRate();

        int newparticles = (int)newParticlesCount;
        newParticlesCount -= newparticles;

        if (newparticles > particles->getMaxRate())
            newparticles = particles->getMaxRate();

        for(int i=0; i<newparticles; i++){
            int particleIndex = particles->findUnusedParticle();
            
            if (particleIndex >= 0){

                for (int init=0; init < particlesInit.size(); init++){
                    particlesInit[init]->execute(particles, particleIndex);
                }

                particles->setParticleLife(particleIndex, 10.0f);
                particles->setParticlePosition(particleIndex, 5, 0, 0);
                particles->setParticleAcceleration(particleIndex, Vector3(0.0f,9.81f * 5, 0.0f));
                particles->setParticleColor(particleIndex, (rand() % 256 / (float)255), (rand() % 256 / (float)255), (rand() % 256 / (float)255), 1.0);
                particles->setParticleSize(particleIndex, 10);
                
            }
            
        }


        for(int i=0; i<particles->getMaxParticles(); i++){
            
            float life = particles->getParticleLife(i);
            
            if(life > 0.0f){
                
                Vector3 velocity = particles->getParticleVelocity(i);
                Vector3 position = particles->getParticlePosition(i);
                Vector3 acceleration = particles->getParticleAcceleration(i);
                
                velocity += acceleration * (float)delta * 0.5f;
                position += velocity * (float)delta;
                life -= delta;

                particles->setParticleLife(i, life);
                particles->setParticleVelocity(i, velocity);
                particles->setParticlePosition(i, position);
                
                //printf("1.Particle %i life %f position %f %f %f\n", i, life, position.x, position.y, position.z);
            }
        }

        particles->updateParticles();

    }

}
