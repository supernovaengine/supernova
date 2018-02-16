#include "ParticleSpriteInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleSpriteInit::ParticleSpriteInit(std::vector<int> frames){
    setFrames(frames);
}

ParticleSpriteInit::ParticleSpriteInit(int minFrame, int maxFrame){
    if (minFrame <= maxFrame){
        for (int i = minFrame; i <= maxFrame; i++){
            frames.push_back(i);
        }
    }
}

ParticleSpriteInit::ParticleSpriteInit(int frame){
    frames.push_back(frame);
}

ParticleSpriteInit::ParticleSpriteInit(const ParticleSpriteInit &particleInit){
    (*this) = particleInit;
}

ParticleSpriteInit::~ParticleSpriteInit(){

}

ParticleSpriteInit& ParticleSpriteInit::operator=(const ParticleSpriteInit &p){
    this->frames = p.frames;

    return *this;
}

bool ParticleSpriteInit::operator==(const ParticleSpriteInit &p){
    return (this->frames == p.frames);
}

bool ParticleSpriteInit::operator!=(const ParticleSpriteInit &p){
    return (this->frames != p.frames);
}

void ParticleSpriteInit::setFrames(std::vector<int> frames){
    this->frames = frames;
}

std::vector<int> ParticleSpriteInit::getFrames(){
    return frames;
}

void ParticleSpriteInit::execute(Particles* particles, int particle){
    int index = int(frames.size()*rand()/(RAND_MAX + 1.0));

    particles->setParticleSprite(particle, frames[index]);
}