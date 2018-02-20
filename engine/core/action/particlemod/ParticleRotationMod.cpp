#include "ParticleRotationMod.h"

using namespace Supernova;

ParticleRotationMod::ParticleRotationMod(float fromLife, float toLife, float fromRotation, float toRotation): ParticleMod(fromLife, toLife){
    setRotation(fromRotation, toRotation);
}

ParticleRotationMod::ParticleRotationMod(const ParticleRotationMod &particleInit){
    (*this) = particleInit;
}

ParticleRotationMod::~ParticleRotationMod(){

}

ParticleRotationMod& ParticleRotationMod::operator=(const ParticleRotationMod &p){
    this->fromRotation = p.fromRotation;
    this->toRotation = p.toRotation;

    return *this;
}

bool ParticleRotationMod::operator==(const ParticleRotationMod &p){
    return (this->fromRotation == p.fromRotation && this->toRotation == p.toRotation &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticleRotationMod::operator!=(const ParticleRotationMod &p){
    return (this->fromRotation != p.fromRotation || this->toRotation != p.toRotation ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticleRotationMod::setRotation(float fromRotation, float toRotation){
    this->fromRotation = fromRotation;
    this->toRotation = toRotation;
}

float ParticleRotationMod::getFromRotation(){
    return fromRotation;
}

float ParticleRotationMod::getToRotation(){
    return toRotation;
}

void ParticleRotationMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        float rotation = fromRotation + ((toRotation - fromRotation) * value);

        particles->setParticleRotation(particle, rotation);
    }
}