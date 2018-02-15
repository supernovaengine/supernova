#include "ParticleVelocityInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleVelocityInit::ParticleVelocityInit(Vector3 minVelocity, Vector3 maxVelocity){
    setVelocity(minVelocity, maxVelocity);
}

ParticleVelocityInit::ParticleVelocityInit(Vector3 velocity){
    setVelocity(velocity);
}

ParticleVelocityInit::~ParticleVelocityInit(){

}

ParticleVelocityInit::ParticleVelocityInit(const ParticleVelocityInit &particleInit){
    (*this) = particleInit;
}

ParticleVelocityInit& ParticleVelocityInit::operator=(const ParticleVelocityInit &p){
    this->minVelocity = p.minVelocity;
    this->maxVelocity = p.maxVelocity;

    return *this;
}

bool ParticleVelocityInit::operator==(const ParticleVelocityInit &p){
    return (this->minVelocity == p.minVelocity && this->maxVelocity == p.maxVelocity);
}

bool ParticleVelocityInit::operator!=(const ParticleVelocityInit &p){
    return (this->minVelocity != p.minVelocity || this->maxVelocity != p.maxVelocity);
}

void ParticleVelocityInit::setVelocity(Vector3 minVelocity, Vector3 maxVelocity){
    this->minVelocity = minVelocity;
    this->maxVelocity = maxVelocity;
}

void ParticleVelocityInit::setVelocity(Vector3 velocity){
    this->minVelocity = velocity;
    this->maxVelocity = velocity;
}

Vector3 ParticleVelocityInit::getMinVelocity(){
    return minVelocity;
}

Vector3 ParticleVelocityInit::getMaxVelocity(){
    return maxVelocity;
}

void ParticleVelocityInit::execute(Particles* particles, int particle){
    Vector3 velocity;

    if (minVelocity != maxVelocity) {
        velocity = Vector3(minVelocity.x + ((maxVelocity.x - minVelocity.x) * (float) rand() / (float) RAND_MAX),
                           minVelocity.y + ((maxVelocity.y - minVelocity.y) * (float) rand() / (float) RAND_MAX),
                           minVelocity.z + ((maxVelocity.z - minVelocity.z) * (float) rand() / (float) RAND_MAX));
    }else{
        velocity = maxVelocity;
    }

    particles->setParticleVelocity(particle, velocity);
}