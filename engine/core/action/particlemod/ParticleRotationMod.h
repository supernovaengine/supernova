#ifndef ANDROID_PARTICLEROTATIONMOD_H
#define ANDROID_PARTICLEROTATIONMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticleRotationMod: public ParticleMod {

    protected:
        float fromRotation;
        float toRotation;

    public:

        ParticleRotationMod(float fromLife, float toLife, float fromRotation, float toRotation);
        ParticleRotationMod(const ParticleRotationMod &particleInit);
        virtual ~ParticleRotationMod();

        ParticleRotationMod& operator=(const ParticleRotationMod &p);
        bool operator==(const ParticleRotationMod &p);
        bool operator!=(const ParticleRotationMod &p);

        void setRotation(float fromRotation, float toRotation);

        float getFromRotation();
        float getToRotation();

        virtual void execute(Particles* particles, int particle, float life);
    };

}

#endif //PARTICLEROTATIONMOD_H
