#include "ParticleSizeMod.h"

using namespace Supernova;

ParticleSizeMod::ParticleSizeMod(float fromLife, float toLife, float fromSize, float toSize): ParticleMod(fromLife, toLife){
    setSize(fromSize, toSize);
}

ParticleSizeMod::ParticleSizeMod(const ParticleSizeMod &particleInit){
    (*this) = particleInit;
}

ParticleSizeMod::~ParticleSizeMod(){

}

ParticleSizeMod& ParticleSizeMod::operator=(const ParticleSizeMod &p){
    this->fromSize = p.fromSize;
    this->toSize = p.toSize;

    return *this;
}

bool ParticleSizeMod::operator==(const ParticleSizeMod &p){
    return (this->fromSize == p.fromSize && this->toSize == p.toSize &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticleSizeMod::operator!=(const ParticleSizeMod &p){
    return (this->fromSize != p.fromSize || this->toSize != p.toSize ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticleSizeMod::setSize(float fromSize, float toSize){
    this->fromSize = fromSize;
    this->toSize = toSize;
}

float ParticleSizeMod::getFromSize(){
    return fromSize;
}

float ParticleSizeMod::getToSize(){
    return toSize;
}

void ParticleSizeMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        float size = fromSize + ((toSize - fromSize) * value);

        particles->setParticleSize(particle, size);
    }
}