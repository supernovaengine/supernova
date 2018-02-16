#ifndef PARTICLEMOD_H
#define PARTICLEMOD_H

#include "Particles.h"

namespace Supernova {

    class ParticleMod {

    protected:
        float fromLife;
        float toLife;

        float value;

    public:
        ParticleMod();
        ParticleMod(float fromLife, float toLife);
        virtual ~ParticleMod();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLEMOD_H
