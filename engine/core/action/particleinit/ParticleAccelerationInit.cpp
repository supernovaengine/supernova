#include "ParticleAccelerationInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleAccelerationInit::ParticleAccelerationInit(Vector3 minAcceleration, Vector3 maxAcceleration){
    setAcceleration(minAcceleration, maxAcceleration);
}

ParticleAccelerationInit::ParticleAccelerationInit(Vector3 acceleration){
    setAcceleration(acceleration);
}

ParticleAccelerationInit::ParticleAccelerationInit(const ParticleAccelerationInit &particleInit){
    (*this) = particleInit;
}

ParticleAccelerationInit::~ParticleAccelerationInit(){

}

ParticleAccelerationInit& ParticleAccelerationInit::operator=(const ParticleAccelerationInit &p){
    this->minAcceleration = p.minAcceleration;
    this->maxAcceleration = p.maxAcceleration;

    return *this;
}

bool ParticleAccelerationInit::operator==(const ParticleAccelerationInit &p){
    return (this->minAcceleration == p.minAcceleration && this->maxAcceleration == p.maxAcceleration);
}

bool ParticleAccelerationInit::operator!=(const ParticleAccelerationInit &p){
    return (this->minAcceleration != p.minAcceleration || this->maxAcceleration != p.maxAcceleration);
}

void ParticleAccelerationInit::setAcceleration(Vector3 minAcceleration, Vector3 maxAcceleration){
    this->minAcceleration = minAcceleration;
    this->maxAcceleration = maxAcceleration;
}

void ParticleAccelerationInit::setAcceleration(Vector3 acceleration){
    this->minAcceleration = acceleration;
    this->maxAcceleration = acceleration;
}

Vector3 ParticleAccelerationInit::getMinAcceleration(){
    return minAcceleration;
}

Vector3 ParticleAccelerationInit::getMaxAcceleration(){
    return maxAcceleration;
}

void ParticleAccelerationInit::execute(Particles* particles, int particle){
    Vector3 acceleration;

    if (minAcceleration != maxAcceleration) {
        acceleration = Vector3(minAcceleration.x + ((maxAcceleration.x - minAcceleration.x) * (float) rand() / (float) RAND_MAX),
                           minAcceleration.y + ((maxAcceleration.y - minAcceleration.y) * (float) rand() / (float) RAND_MAX),
                           minAcceleration.z + ((maxAcceleration.z - minAcceleration.z) * (float) rand() / (float) RAND_MAX));
    }else{
        acceleration = maxAcceleration;
    }

    particles->setParticleAcceleration(particle, acceleration);
}