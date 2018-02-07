#ifndef PARTICLEVELOCITYINIT_H
#define PARTICLEVELOCITYINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleVelocityInit: public ParticleInit {

    public:

        ParticleVelocityInit();
        virtual ~ParticleVelocityInit();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEVELOCITYINIT_H
