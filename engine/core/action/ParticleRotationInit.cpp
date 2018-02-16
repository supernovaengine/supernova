#include "ParticleRotationInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleRotationInit::ParticleRotationInit(float minRotation, float maxRotation){
    setRotation(minRotation, maxRotation);
}

ParticleRotationInit::ParticleRotationInit(float rotation){
    setRotation(rotation);
}

ParticleRotationInit::ParticleRotationInit(const ParticleRotationInit &particleInit){
    (*this) = particleInit;
}

ParticleRotationInit::~ParticleRotationInit(){

}

ParticleRotationInit& ParticleRotationInit::operator=(const ParticleRotationInit &p){
    this->minRotation = p.minRotation;
    this->maxRotation = p.maxRotation;

    return *this;
}

bool ParticleRotationInit::operator==(const ParticleRotationInit &p){
    return (this->minRotation == p.minRotation && this->maxRotation == p.maxRotation);
}

bool ParticleRotationInit::operator!=(const ParticleRotationInit &p){
    return (this->minRotation != p.minRotation || this->maxRotation != p.maxRotation);
}

void ParticleRotationInit::setRotation(float minRotation, float maxRotation){
    this->minRotation = minRotation;
    this->maxRotation = maxRotation;
}

void ParticleRotationInit::setRotation(float rotation){
    this->minRotation = rotation;
    this->maxRotation = rotation;
}

float ParticleRotationInit::getMinRotation(){
    return minRotation;
}

float ParticleRotationInit::getMaxRotation(){
    return maxRotation;
}

void ParticleRotationInit::execute(Particles* particles, int particle){
    float rotation;

    if (minRotation != maxRotation) {
        rotation = minRotation + ((maxRotation - minRotation) * (float) rand() / (float) RAND_MAX);
    } else {
        rotation = maxRotation;
    }

    particles->setParticleRotation(particle, rotation);
}