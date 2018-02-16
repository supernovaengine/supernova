#ifndef PARTICLESPRITEINIT_H
#define PARTICLESPRITEINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleSpriteInit: public ParticleInit {

    protected:
        std::vector<int> frames;

    public:

        ParticleSpriteInit(std::vector<int> frames);
        ParticleSpriteInit(int minFrame, int maxFrame);
        ParticleSpriteInit(int frame);
        ParticleSpriteInit(const ParticleSpriteInit &particleInit);
        virtual ~ParticleSpriteInit();

        ParticleSpriteInit& operator=(const ParticleSpriteInit &p);
        bool operator==(const ParticleSpriteInit &p);
        bool operator!=(const ParticleSpriteInit &p);

        void setFrames(std::vector<int> frames);

        std::vector<int> getFrames();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLESPRITEINIT_H
