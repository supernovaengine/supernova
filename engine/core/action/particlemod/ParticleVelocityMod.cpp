#include "ParticleVelocityMod.h"


using namespace Supernova;

ParticleVelocityMod::ParticleVelocityMod(float fromLife, float toLife, Vector3 fromVelocity, Vector3 toVelocity): ParticleMod(fromLife, toLife){
    setVelocity(fromVelocity, toVelocity);
}

ParticleVelocityMod::ParticleVelocityMod(const ParticleVelocityMod &particleInit){
    (*this) = particleInit;
}

ParticleVelocityMod::~ParticleVelocityMod(){

}

ParticleVelocityMod& ParticleVelocityMod::operator=(const ParticleVelocityMod &p){
    this->fromVelocity = p.fromVelocity;
    this->toVelocity = p.toVelocity;

    return *this;
}

bool ParticleVelocityMod::operator==(const ParticleVelocityMod &p){
    return (this->fromVelocity == p.fromVelocity && this->toVelocity == p.toVelocity &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticleVelocityMod::operator!=(const ParticleVelocityMod &p){
    return (this->fromVelocity != p.fromVelocity || this->toVelocity != p.toVelocity ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticleVelocityMod::setVelocity(Vector3 fromVelocity, Vector3 toVelocity){
    this->fromVelocity = fromVelocity;
    this->toVelocity = toVelocity;
}

Vector3 ParticleVelocityMod::getFromVelocity(){
    return fromVelocity;
}

Vector3 ParticleVelocityMod::getToVelocity(){
    return toVelocity;
}

void ParticleVelocityMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        Vector3 velocity = fromVelocity + ((toVelocity - fromVelocity) * value);
        particles->setParticleVelocity(particle, velocity);
    }
}