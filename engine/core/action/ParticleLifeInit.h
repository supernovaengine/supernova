#ifndef PARTICLELIFEINIT_H
#define PARTICLELIFEINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleLifeInit: public ParticleInit {

    protected:
        float minLife;
        float maxLife;

    public:

        ParticleLifeInit(float minLife, float maxLife);
        ParticleLifeInit(float life);
        ParticleLifeInit(const ParticleLifeInit &particleInit);
        virtual ~ParticleLifeInit();

        ParticleLifeInit& operator=(const ParticleLifeInit &p);
        bool operator==(const ParticleLifeInit &p);
        bool operator!=(const ParticleLifeInit &p);

        void setLife(float minLife, float maxLife);
        void setLife(float life);

        float getMinLife();
        float getMaxLife();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLELIFEINIT_H
