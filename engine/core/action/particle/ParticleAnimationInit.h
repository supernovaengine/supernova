#ifndef PARTICLEANIMATIONINIT_H
#define PARTICLEANIMATIONINIT_H

#include "Particles.h"

namespace Supernova {

    class ParticleAnimationInit {

    public:

        ParticleAnimationInit();
        virtual ~ParticleAnimationInit();

        virtual void execute(Particles* particles, int particle) = 0;
    };

}


#endif //PARTICLEANIMATIONINIT_H
