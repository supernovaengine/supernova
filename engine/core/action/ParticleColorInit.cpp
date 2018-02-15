#include "ParticleColorInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleColorInit::ParticleColorInit(Vector4 minColor, Vector4 maxColor){
    setColor(minColor, maxColor);
}

ParticleColorInit::ParticleColorInit(Vector4 color){
    setColor(color);
}

ParticleColorInit::ParticleColorInit(float minRed, float minGreen, float minBlue, float maxRed, float maxGreen, float maxBlue){
    setColor(minRed, minGreen, minBlue, maxRed, maxGreen, maxBlue);
}

ParticleColorInit::ParticleColorInit(float red, float green, float blue){
    setColor(red, green, blue);
}

ParticleColorInit::ParticleColorInit(const ParticleColorInit &particleInit){
    (*this) = particleInit;
}

ParticleColorInit::~ParticleColorInit(){

}

ParticleColorInit& ParticleColorInit::operator=(const ParticleColorInit &p){
    this->minColor = p.minColor;
    this->maxColor = p.maxColor;
    this->useAlpha = p.useAlpha;

    return *this;
}

bool ParticleColorInit::operator==(const ParticleColorInit &p){
    return (this->minColor == p.minColor && this->maxColor == p.maxColor && this->useAlpha == p.useAlpha);
}

bool ParticleColorInit::operator!=(const ParticleColorInit &p){
    return (this->minColor != p.minColor || this->maxColor != p.maxColor || this->useAlpha != p.useAlpha);
}

void ParticleColorInit::setColor(Vector4 minColor, Vector4 maxColor){
    this->minColor = minColor;
    this->maxColor = maxColor;
    this->useAlpha = true;
}

void ParticleColorInit::setColor(Vector4 color){
    this->minColor = color;
    this->maxColor = color;
    this->useAlpha = true;
}

void ParticleColorInit::setColor(float minRed, float minGreen, float minBlue, float maxRed, float maxGreen, float maxBlue){
    this->minColor = Vector4(minRed, minGreen, minBlue, 0.0);
    this->maxColor = Vector4(maxRed, maxGreen, maxBlue, 0.0);
    this->useAlpha = false;
}

void ParticleColorInit::setColor(float red, float green, float blue){
    this->minColor = Vector4(red, green, blue, 0.0);
    this->maxColor = Vector4(red, green, blue, 0.0);
    this->useAlpha = false;
}

Vector4 ParticleColorInit::getMinColor(){
    return minColor;
}

Vector4 ParticleColorInit::getMaxColor(){
    return maxColor;
}

void ParticleColorInit::execute(Particles* particles, int particle){
    Vector4 color;

    if (minColor != maxColor) {
        color = Vector4(minColor.x + ((maxColor.x - minColor.x) * (float) rand() / (float) RAND_MAX),
                        minColor.y + ((maxColor.y - minColor.y) * (float) rand() / (float) RAND_MAX),
                        minColor.z + ((maxColor.z - minColor.z) * (float) rand() / (float) RAND_MAX),
                        minColor.w + ((maxColor.w - minColor.w) * (float) rand() / (float) RAND_MAX));
    }else{
        color = maxColor;
    }

    if (!useAlpha){
        float actualAlpha = particles->getParticleColor(particle).w;
        color = Vector4(color.x, color.y, color.z, actualAlpha);
    }

    particles->setParticleColor(particle, color);
}