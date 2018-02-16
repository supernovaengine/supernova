#ifndef PARTICLESIZEINIT_H
#define PARTICLESIZEINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleSizeInit: public ParticleInit {

    protected:
        float minSize;
        float maxSize;

    public:

        ParticleSizeInit(float minSize, float maxSize);
        ParticleSizeInit(float alpha);
        ParticleSizeInit(const ParticleSizeInit &particleInit);
        virtual ~ParticleSizeInit();

        ParticleSizeInit& operator=(const ParticleSizeInit &p);
        bool operator==(const ParticleSizeInit &p);
        bool operator!=(const ParticleSizeInit &p);

        void setSize(float minSize, float maxSize);
        void setSize(float size);

        float getMinSize();
        float getMaxSize();

        virtual void execute(Particles* particles, int particle);
    };

}



#endif
