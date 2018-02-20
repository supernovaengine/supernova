#ifndef PARTICLEALPHAMOD_H
#define PARTICLEALPHAMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticleAlphaMod: public ParticleMod {

    protected:
        float fromAlpha;
        float toAlpha;

    public:

        ParticleAlphaMod(float fromLife, float toLife, float fromAlpha, float toAlpha);
        ParticleAlphaMod(const ParticleAlphaMod &particleInit);
        virtual ~ParticleAlphaMod();

        ParticleAlphaMod& operator=(const ParticleAlphaMod &p);
        bool operator==(const ParticleAlphaMod &p);
        bool operator!=(const ParticleAlphaMod &p);

        void setAlpha(float fromAlpha, float toAlpha);

        float getFromAlpha();
        float getToAlpha();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLEALPHAMOD_H
