#include "ParticlePositionInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticlePositionInit::ParticlePositionInit(Vector3 minPosition, Vector3 maxPosition){
    setPosition(minPosition, maxPosition);
}

ParticlePositionInit::ParticlePositionInit(Vector3 position){
    setPosition(position);
}

ParticlePositionInit::~ParticlePositionInit(){

}

ParticlePositionInit::ParticlePositionInit(const ParticlePositionInit &particleInit){
    (*this) = particleInit;
}

ParticlePositionInit& ParticlePositionInit::operator=(const ParticlePositionInit &p){
    this->minPosition = p.minPosition;
    this->maxPosition = p.maxPosition;

    return *this;
}

bool ParticlePositionInit::operator==(const ParticlePositionInit &p){
    return (this->minPosition == p.minPosition && this->maxPosition == p.maxPosition);
}

bool ParticlePositionInit::operator!=(const ParticlePositionInit &p){
    return (this->minPosition != p.minPosition || this->maxPosition != p.maxPosition);
}

void ParticlePositionInit::setPosition(Vector3 minPosition, Vector3 maxPosition){
    this->minPosition = minPosition;
    this->maxPosition = maxPosition;
}

void ParticlePositionInit::setPosition(Vector3 position){
    this->minPosition = position;
    this->maxPosition = position;
}

Vector3 ParticlePositionInit::getMinPosition(){
    return minPosition;
}

Vector3 ParticlePositionInit::getMaxPosition(){
    return maxPosition;
}

void ParticlePositionInit::execute(Particles* particles, int particle){
    Vector3 position;

    if (minPosition != maxPosition) {
        position = Vector3(minPosition.x + ((maxPosition.x - minPosition.x) * (float) rand() / (float) RAND_MAX),
                           minPosition.y + ((maxPosition.y - minPosition.y) * (float) rand() / (float) RAND_MAX),
                           minPosition.z + ((maxPosition.z - minPosition.z) * (float) rand() / (float) RAND_MAX));
    }else{
        position = maxPosition;
    }

    particles->setParticlePosition(particle, position);
}