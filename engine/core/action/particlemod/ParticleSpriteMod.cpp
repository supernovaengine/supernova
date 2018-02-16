#include "ParticleSpriteMod.h"


using namespace Supernova;

ParticleSpriteMod::ParticleSpriteMod(float fromLife, float toLife, std::vector<int> frames): ParticleMod(fromLife, toLife){
    setFrames(frames);
}

ParticleSpriteMod::ParticleSpriteMod(float fromLife, float toLife, int minFrame, int maxFrame): ParticleMod(fromLife, toLife){
    if (minFrame <= maxFrame){
        for (int i = minFrame; i <= maxFrame; i++){
            frames.push_back(i);
        }
    }
}

ParticleSpriteMod::ParticleSpriteMod(float fromLife, float toLife, int frame): ParticleMod(fromLife, toLife){
    frames.push_back(frame);
}

ParticleSpriteMod::ParticleSpriteMod(const ParticleSpriteMod &particleInit){
    (*this) = particleInit;
}

ParticleSpriteMod::~ParticleSpriteMod(){

}

ParticleSpriteMod& ParticleSpriteMod::operator=(const ParticleSpriteMod &p){
    this->frames = p.frames;

    return *this;
}

bool ParticleSpriteMod::operator==(const ParticleSpriteMod &p){
    return (this->frames == p.frames &&
            this->fromLife == p.fromLife && this->toLife == p.toLife);
}

bool ParticleSpriteMod::operator!=(const ParticleSpriteMod &p){
    return (this->frames != p.frames ||
            this->fromLife != p.fromLife || this->toLife != p.toLife);
}

void ParticleSpriteMod::setFrames(std::vector<int> frames){
    this->frames = frames;
}

std::vector<int> ParticleSpriteMod::getFrames(){
    return frames;
}

void ParticleSpriteMod::execute(Particles* particles, int particle, float life) {
    ParticleMod::execute(particles, particle, life);

    if (value >= 0 && value <= 1){
        int index = (int)(frames.size() * value);
        particles->setParticleSprite(particle, frames[index]);
    }
}