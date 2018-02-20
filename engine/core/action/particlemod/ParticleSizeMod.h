#ifndef PARTICLESIZEMOD_H
#define PARTICLESIZEMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticleSizeMod: public ParticleMod {

    protected:
        float fromSize;
        float toSize;

    public:

        ParticleSizeMod(float fromLife, float toLife, float fromSize, float toSize);
        ParticleSizeMod(const ParticleSizeMod &particleInit);
        virtual ~ParticleSizeMod();

        ParticleSizeMod& operator=(const ParticleSizeMod &p);
        bool operator==(const ParticleSizeMod &p);
        bool operator!=(const ParticleSizeMod &p);

        void setSize(float fromSize, float toSize);

        float getFromSize();
        float getToSize();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLESIZEMOD_H
