#ifndef PARTICLEVELOCITYINIT_H
#define PARTICLEVELOCITYINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleVelocityInit: public ParticleInit {

    protected:
        Vector3 minVelocity;
        Vector3 maxVelocity;

    public:

        ParticleVelocityInit(Vector3 minVelocity, Vector3 maxVelocity);
        ParticleVelocityInit(Vector3 velocity);
        ParticleVelocityInit(const ParticleVelocityInit &particleInit);
        virtual ~ParticleVelocityInit();

        ParticleVelocityInit& operator=(const ParticleVelocityInit &p);
        bool operator==(const ParticleVelocityInit &p);
        bool operator!=(const ParticleVelocityInit &p);

        void setVelocity(Vector3 minVelocity, Vector3 maxVelocity);
        void setVelocity(Vector3 velocity);

        Vector3 getMinVelocity();
        Vector3 getMaxVelocity();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEVELOCITYINIT_H
