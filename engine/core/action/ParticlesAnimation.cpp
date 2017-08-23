#include "ParticlesAnimation.h"


using namespace Supernova;


ParticlesAnimation::ParticlesAnimation(){
    
}
ParticlesAnimation::~ParticlesAnimation(){
    
}

void ParticlesAnimation::play(){
    Action::play();
}

void ParticlesAnimation::stop(){
    Action::stop();
}

void ParticlesAnimation::reset(){
    Action::reset();
}

void ParticlesAnimation::step(){
    Action::step();
}
