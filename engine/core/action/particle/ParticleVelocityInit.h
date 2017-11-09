#ifndef PARTICLEVELOCITYINIT_H
#define PARTICLEVELOCITYINIT_H

#include "ParticleAnimationInit.h"

namespace Supernova {

    class ParticleVelocityInit: public ParticleAnimationInit {

    public:

        ParticleVelocityInit();
        virtual ~ParticleVelocityInit();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEVELOCITYINIT_H
