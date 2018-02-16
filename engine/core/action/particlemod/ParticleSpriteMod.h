#ifndef PARTICLESPRITEMOD_H
#define PARTICLESPRITEMOD_H

#include "ParticleMod.h"

namespace Supernova {

    class ParticleSpriteMod: public ParticleMod {

    protected:

        std::vector<int> frames;

    public:

        ParticleSpriteMod(float fromLife, float toLife, std::vector<int> frames);
        ParticleSpriteMod(float fromLife, float toLife, int minFrame, int maxFrame);
        ParticleSpriteMod(float fromLife, float toLife, int frame);
        ParticleSpriteMod(const ParticleSpriteMod &particleInit);
        virtual ~ParticleSpriteMod();

        ParticleSpriteMod& operator=(const ParticleSpriteMod &p);
        bool operator==(const ParticleSpriteMod &p);
        bool operator!=(const ParticleSpriteMod &p);

        void setFrames(std::vector<int> frames);

        std::vector<int> getFrames();

        virtual void execute(Particles* particles, int particle, float life);
    };

}


#endif //PARTICLESPRITEMOD_H
