#include "ParticleColorMod.h"

using namespace Supernova;

ParticleColorMod::ParticleColorMod(float fromLife, float toLife, Vector4 fromColor, Vector4 toColor): ParticleMod(fromLife, toLife){
    setColor(fromColor, toColor);
}

ParticleColorMod::ParticleColorMod(float fromLife, float toLife, float fromRed, float fromGreen, float fromBlue, float toRed, float toGreen, float toBlue): ParticleMod(fromLife, toLife){
    setColor(fromRed, fromGreen, fromBlue, toRed, toGreen, toBlue);
}

ParticleColorMod::ParticleColorMod(const ParticleColorMod &particleInit){
    (*this) = particleInit;
}

ParticleColorMod::~ParticleColorMod(){

}

ParticleColorMod& ParticleColorMod::operator=(const ParticleColorMod &p){
    this->fromColor = p.fromColor;
    this->toColor = p.toColor;

    return *this;
}

bool ParticleColorMod::operator==(const ParticleColorMod &p){
    return (this->fromColor == p.fromColor && this->toColor == p.toColor &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticleColorMod::operator!=(const ParticleColorMod &p){
    return (this->fromColor != p.fromColor || this->toColor != p.toColor ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticleColorMod::setColor(Vector4 fromColor, Vector4 toColor){
    this->fromColor = fromColor;
    this->toColor = toColor;
    this->useAlpha = true;
}

void ParticleColorMod::setColor(float fromRed, float fromGreen, float fromBlue, float toRed, float toGreen, float toBlue){
    this->fromColor = Vector4(fromRed, fromGreen, fromBlue, 0.0);
    this->toColor = Vector4(toRed, toGreen, toBlue, 0.0);
    this->useAlpha = false;
}

Vector4 ParticleColorMod::getFromColor(){
    return fromColor;
}

Vector4 ParticleColorMod::getToColor(){
    return toColor;
}

void ParticleColorMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        Vector4 color = fromColor + ((toColor - fromColor) * value);
        if (!useAlpha){
            float actualAlpha = particles->getParticleColor(particle).w;
            color = Vector4(color.x, color.y, color.z, actualAlpha);
        }
        particles->setParticleColor(particle, color);
    }
}