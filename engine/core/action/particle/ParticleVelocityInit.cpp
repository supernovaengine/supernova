#include "ParticleVelocityInit.h"

#include <stdlib.h>

using namespace Supernova;

ParticleVelocityInit::ParticleVelocityInit(){

}

ParticleVelocityInit::~ParticleVelocityInit(){

}

void ParticleVelocityInit::execute(Particles* particles, int particle){

    Vector3 maindir(0.0f, 10.0f, 0.0f);


    Vector3 randomdir((rand()%2000 - 1000.0f)/100.0f,
                      (rand()%2000 - 1000.0f)/100.0f,
                      (rand()%2000 - 1000.0f)/100.0f
    );

    float spread = 1.5f;

    particles->setParticleVelocity(particle, maindir + randomdir*spread);
}