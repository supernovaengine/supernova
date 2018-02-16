#ifndef PARTICLEVELOCITYMOD_H
#define PARTICLEVELOCITYMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticleVelocityMod: public ParticleMod {

    protected:
        Vector3 fromVelocity;
        Vector3 toVelocity;

    public:

        ParticleVelocityMod(float fromLife, float toLife, Vector3 fromVelocity, Vector3 toVelocity);
        ParticleVelocityMod(const ParticleVelocityMod &particleInit);
        virtual ~ParticleVelocityMod();

        ParticleVelocityMod& operator=(const ParticleVelocityMod &p);
        bool operator==(const ParticleVelocityMod &p);
        bool operator!=(const ParticleVelocityMod &p);

        void setVelocity(Vector3 fromVelocity, Vector3 toVelocity);

        Vector3 getFromVelocity();
        Vector3 getToVelocity();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLEVELOCITYMOD_H
