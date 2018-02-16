#include "ParticleSizeInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleSizeInit::ParticleSizeInit(float minSize, float maxSize){
    setSize(minSize, maxSize);
}

ParticleSizeInit::ParticleSizeInit(float size){
    setSize(size);
}

ParticleSizeInit::~ParticleSizeInit(){

}

ParticleSizeInit::ParticleSizeInit(const ParticleSizeInit &particleInit){
    (*this) = particleInit;
}

ParticleSizeInit& ParticleSizeInit::operator=(const ParticleSizeInit &p){
    this->minSize = p.minSize;
    this->maxSize = p.maxSize;

    return *this;
}

bool ParticleSizeInit::operator==(const ParticleSizeInit &p){
    return (this->minSize == p.minSize && this->maxSize == p.maxSize);
}

bool ParticleSizeInit::operator!=(const ParticleSizeInit &p){
    return (this->minSize != p.minSize || this->maxSize != p.maxSize);
}

void ParticleSizeInit::setSize(float minSize, float maxSize){
    this->minSize = minSize;
    this->maxSize = maxSize;
}

void ParticleSizeInit::setSize(float size){
    this->minSize = size;
    this->maxSize = size;
}

float ParticleSizeInit::getMinSize(){
    return minSize;
}

float ParticleSizeInit::getMaxSize(){
    return maxSize;
}

void ParticleSizeInit::execute(Particles* particles, int particle){
    float size;

    if (minSize != maxSize) {
        size = minSize + ((maxSize - minSize) * (float) rand() / (float) RAND_MAX);
    }else{
        size = maxSize;
    }

    particles->setParticleSize(particle, size);
}