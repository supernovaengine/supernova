#include "ParticleAlphaMod.h"

using namespace Supernova;

ParticleAlphaMod::ParticleAlphaMod(float fromLife, float toLife, float fromAlpha, float toAlpha): ParticleMod(fromLife, toLife){
    setAlpha(fromAlpha, toAlpha);
}

ParticleAlphaMod::ParticleAlphaMod(const ParticleAlphaMod &particleInit){
    (*this) = particleInit;
}

ParticleAlphaMod::~ParticleAlphaMod(){

}

ParticleAlphaMod& ParticleAlphaMod::operator=(const ParticleAlphaMod &p){
    this->fromAlpha = p.fromAlpha;
    this->toAlpha = p.toAlpha;

    return *this;
}

bool ParticleAlphaMod::operator==(const ParticleAlphaMod &p){
    return (this->fromAlpha == p.fromAlpha && this->toAlpha == p.toAlpha &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticleAlphaMod::operator!=(const ParticleAlphaMod &p){
    return (this->fromAlpha != p.fromAlpha || this->toAlpha != p.toAlpha ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticleAlphaMod::setAlpha(float fromAlpha, float toAlpha){
    this->fromAlpha = fromAlpha;
    this->toAlpha = toAlpha;
}

float ParticleAlphaMod::getFromAlpha(){
    return fromAlpha;
}

float ParticleAlphaMod::getToAlpha(){
    return toAlpha;
}

void ParticleAlphaMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        float alpha = fromAlpha + ((toAlpha - fromAlpha) * value);

        Vector4 actualColor = particles->getParticleColor(particle);
        Vector4 color = Vector4(actualColor.x, actualColor.y, actualColor.z, alpha);

        particles->setParticleColor(particle, color);
    }
}