#include "ParticleAlphaInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleAlphaInit::ParticleAlphaInit(float minAlpha, float maxAlpha){
    setAlpha(minAlpha, maxAlpha);
}

ParticleAlphaInit::ParticleAlphaInit(float alpha){
    setAlpha(alpha);
}

ParticleAlphaInit::ParticleAlphaInit(const ParticleAlphaInit &particleInit){
    (*this) = particleInit;
}

ParticleAlphaInit::~ParticleAlphaInit(){

}

ParticleAlphaInit& ParticleAlphaInit::operator=(const ParticleAlphaInit &p){
    this->minAlpha = p.minAlpha;
    this->maxAlpha = p.maxAlpha;

    return *this;
}

bool ParticleAlphaInit::operator==(const ParticleAlphaInit &p){
    return (this->minAlpha == p.minAlpha && this->maxAlpha == p.maxAlpha);
}

bool ParticleAlphaInit::operator!=(const ParticleAlphaInit &p){
    return (this->minAlpha != p.minAlpha || this->maxAlpha != p.maxAlpha);
}

void ParticleAlphaInit::setAlpha(float minAlpha, float maxAlpha){
    this->minAlpha = minAlpha;
    this->maxAlpha = maxAlpha;
}

void ParticleAlphaInit::setAlpha(float alpha){
    this->minAlpha = alpha;
    this->maxAlpha = alpha;
}

float ParticleAlphaInit::getMinAlpha(){
    return minAlpha;
}

float ParticleAlphaInit::getMaxAlpha(){
    return maxAlpha;
}

void ParticleAlphaInit::execute(Particles* particles, int particle){
    float alpha;

    if (minAlpha != maxAlpha) {
        alpha = minAlpha + ((maxAlpha - minAlpha) * (float) rand() / (float) RAND_MAX);
    }else{
        alpha = maxAlpha;
    }

    Vector4 actualColor = particles->getParticleColor(particle);
    particles->setParticleColor(particle, Vector4(actualColor.x, actualColor.y, actualColor.z, alpha));
}