#include "ParticlePositionMod.h"

using namespace Supernova;

ParticlePositionMod::ParticlePositionMod(float fromLife, float toLife, Vector3 fromPosition, Vector3 toPosition): ParticleMod(fromLife, toLife){
    setPosition(fromPosition, toPosition);
}

ParticlePositionMod::ParticlePositionMod(const ParticlePositionMod &particleInit){
    (*this) = particleInit;
}

ParticlePositionMod::~ParticlePositionMod(){

}

ParticlePositionMod& ParticlePositionMod::operator=(const ParticlePositionMod &p){
    this->fromPosition = p.fromPosition;
    this->toPosition = p.toPosition;

    return *this;
}

bool ParticlePositionMod::operator==(const ParticlePositionMod &p){
    return (this->fromPosition == p.fromPosition && this->toPosition == p.toPosition &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticlePositionMod::operator!=(const ParticlePositionMod &p){
    return (this->fromPosition != p.fromPosition || this->toPosition != p.toPosition ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticlePositionMod::setPosition(Vector3 fromPosition, Vector3 toPosition){
    this->fromPosition = fromPosition;
    this->toPosition = toPosition;
}

Vector3 ParticlePositionMod::getFromPosition(){
    return fromPosition;
}

Vector3 ParticlePositionMod::getToPosition(){
    return toPosition;
}

void ParticlePositionMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        Vector3 position = fromPosition + ((toPosition - fromPosition) * value);
        particles->setParticlePosition(particle, position);
    }
}