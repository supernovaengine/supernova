#ifndef PARTICLECOLORMOD_H
#define PARTICLECOLORMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticleColorMod: public ParticleMod {

    protected:
        Vector4 fromColor;
        Vector4 toColor;

        bool useAlpha;

    public:

        ParticleColorMod(float fromLife, float toLife, Vector4 fromVelocity, Vector4 toVelocity);
        ParticleColorMod(float fromLife, float toLife, float fromRed, float fromGreen, float fromBlue, float toRed, float toGreen, float toBlue);
        ParticleColorMod(const ParticleColorMod &particleInit);
        virtual ~ParticleColorMod();

        ParticleColorMod& operator=(const ParticleColorMod &p);
        bool operator==(const ParticleColorMod &p);
        bool operator!=(const ParticleColorMod &p);

        void setColor(Vector4 fromVelocity, Vector4 toVelocity);
        void setColor(float fromRed, float fromGreen, float fromBlue, float toRed, float toGreen, float toBlue);

        Vector4 getFromColor();
        Vector4 getToColor();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLECOLORMOD_H
