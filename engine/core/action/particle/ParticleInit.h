#ifndef PARTICLEINIT_H
#define PARTICLEINIT_H

#include "Particles.h"

namespace Supernova {

    class ParticleInit {

    public:

        ParticleInit();
        virtual ~ParticleInit();

        virtual void execute(Particles* particles, int particle) = 0;
    };

}


#endif //PARTICLEINIT_H
