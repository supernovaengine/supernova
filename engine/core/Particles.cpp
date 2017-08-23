#include "Particles.h"

using namespace Supernova;

Particles::Particles(): Points(){
    this->numParticles = 50;
    this->lastUsedParticle = 0;
}

Particles::Particles(int numParticles): Particles(){
    this->numParticles = numParticles;
}


Particles::~Particles(){
    
}

void Particles::setNumParticles(int numParticles){
    this->numParticles = numParticles;
}

int Particles::getNumParticles(){
    return numParticles;
}

void Particles::createParticles(){
    
    clearPoints();
    particlesData.clear();
    
    for (int i = 0; i < numParticles; i++){
        addPoint();
        particlesData.push_back({5});
    }
}

int Particles::findUnusedParticle(){
    
    for (int i=lastUsedParticle; i<numParticles; i++){
        if (particlesData[i].life < 0){
            lastUsedParticle = i;
            return i;
        }
    }
    
    for (int i=0; i<lastUsedParticle; i++){
        if (particlesData[i].life < 0){
            lastUsedParticle = i;
            return i;
        }
    }
    
    return 0;
}

bool Particles::load(){
    
    createParticles();
    
    return Points::load();
}
