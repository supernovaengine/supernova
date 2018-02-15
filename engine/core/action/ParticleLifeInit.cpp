#include "ParticleLifeInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleLifeInit::ParticleLifeInit(float minLife, float maxLife){
    setLife(minLife, maxLife);
}

ParticleLifeInit::ParticleLifeInit(float life){
    setLife(life);
}

ParticleLifeInit::ParticleLifeInit(const ParticleLifeInit &particleInit){
    (*this) = particleInit;
}

ParticleLifeInit::~ParticleLifeInit(){

}

ParticleLifeInit& ParticleLifeInit::operator=(const ParticleLifeInit &p){
    this->minLife = p.minLife;
    this->maxLife = p.maxLife;

    return *this;
}

bool ParticleLifeInit::operator==(const ParticleLifeInit &p){
    return (this->minLife == p.minLife && this->maxLife == p.maxLife);
}

bool ParticleLifeInit::operator!=(const ParticleLifeInit &p){
    return (this->minLife != p.minLife || this->maxLife != p.maxLife);
}

void ParticleLifeInit::setLife(float minLife, float maxLife){
    this->minLife = minLife;
    this->maxLife = maxLife;
}

void ParticleLifeInit::setLife(float life){
    this->minLife = life;
    this->maxLife = life;
}

float ParticleLifeInit::getMinLife(){
    return minLife;
}

float ParticleLifeInit::getMaxLife(){
    return maxLife;
}

void ParticleLifeInit::execute(Particles* particles, int particle){
    float life;

    if (minLife != maxLife) {
        life = minLife + ((maxLife - minLife) * (float) rand() / (float) RAND_MAX);
    }else{
        life = maxLife;
    }

    particles->setParticleLife(particle, life);
}