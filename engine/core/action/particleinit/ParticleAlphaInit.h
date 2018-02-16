#ifndef PARTICLEALPHAINIT_H
#define PARTICLEALPHAINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleAlphaInit: public ParticleInit {

    protected:
        float minAlpha;
        float maxAlpha;

    public:

        ParticleAlphaInit(float minAlpha, float maxAlpha);
        ParticleAlphaInit(float alpha);
        ParticleAlphaInit(const ParticleAlphaInit &particleInit);
        virtual ~ParticleAlphaInit();

        ParticleAlphaInit& operator=(const ParticleAlphaInit &p);
        bool operator==(const ParticleAlphaInit &p);
        bool operator!=(const ParticleAlphaInit &p);

        void setAlpha(float minAlpha, float maxAlpha);
        void setAlpha(float alpha);

        float getMinAlpha();
        float getMaxAlpha();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEALPHAINIT_H
