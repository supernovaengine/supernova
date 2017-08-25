#include "ParticlesAnimation.h"
#include "Particles.h"
#include "platform/Log.h"

#include <stdlib.h> 

using namespace Supernova;


ParticlesAnimation::ParticlesAnimation(): Action(-1, true){
    
}

ParticlesAnimation::~ParticlesAnimation(){
    
}

void ParticlesAnimation::play(){
    Action::play();
    
    if (dynamic_cast<Particles*>(object) == NULL) {
        Log::Error(LOG_TAG, "Object in ParticlesAnimation must be a Particles type");
        stop();
    }
}

void ParticlesAnimation::stop(){
    Action::stop();
}

void ParticlesAnimation::reset(){
    Action::reset();
}

void ParticlesAnimation::step(){
    Action::step();
    
    if (object){
        
        Particles* particles = (Particles*)object;
        
        float delta = (float)steptime / 1000;
    
        int newparticles = (int)(delta*10000.0);
        if (newparticles > (int)(0.016f*10000.0))
            newparticles = (int)(0.016f*10000.0);
        
        for(int i=0; i<newparticles; i++){
            int particleIndex = particles->findUnusedParticle();
            
            if (particleIndex >= 0){
            
                float spread = 1.5f;
                Vector3 maindir(0.0f, 10.0f, 0.0f);
                

                Vector3 randomdir((rand()%2000 - 1000.0f)/100.0f,
                                  (rand()%2000 - 1000.0f)/100.0f,
                                  (rand()%2000 - 1000.0f)/100.0f
                                  );
                
                particles->setParticleLife(particleIndex, 10.0f);
                particles->setParticlePosition(particleIndex, 5, 0, 0);
                particles->setParticleVelocity(particleIndex, maindir + randomdir*spread);
                particles->setParticleColor(particleIndex, (rand() % 256 / (float)255), (rand() % 256 / (float)255), (rand() % 256 / (float)255), 1.0);
                particles->setParticleSize(particleIndex, 50);
                
            }
            
        }
        
        
        for(int i=0; i<particles->getNumParticles(); i++){
            
            float life = particles->getParticleLife(i);
            float delta = (float)steptime / 1000;
            
            if(life > 0.0f){
                
                Vector3 velocity = particles->getParticleVelocity(i);
                Vector3 position = particles->getParticlePosition(i);
                
                velocity += Vector3(0.0f,9.81f, 0.0f) * (float)delta * 0.5f;
                position += velocity * (float)delta;
                life -= delta;

                particles->setParticleLife(i, life);
                particles->setParticleVelocity(i, velocity);
                particles->setParticlePosition(i, position);
                
                //printf("1.Particle %i life %f position %f %f %f\n", i, life, position.x, position.y, position.z);
                
            }
        }
        
    }
}
