#ifndef PARTICLEPOSITIONMOD_H
#define PARTICLEPOSITIONMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticlePositionMod: public ParticleMod {

    protected:
        Vector3 fromPosition;
        Vector3 toPosition;

    public:

        ParticlePositionMod(float fromLife, float toLife, Vector3 fromPosition, Vector3 toPosition);
        ParticlePositionMod(const ParticlePositionMod &particleInit);
        virtual ~ParticlePositionMod();

        ParticlePositionMod& operator=(const ParticlePositionMod &p);
        bool operator==(const ParticlePositionMod &p);
        bool operator!=(const ParticlePositionMod &p);

        void setPosition(Vector3 fromPosition, Vector3 toPosition);

        Vector3 getFromPosition();
        Vector3 getToPosition();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLEPOSITIONMOD_H
