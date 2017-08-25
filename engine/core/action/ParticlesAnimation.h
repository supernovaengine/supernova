#ifndef ParticlesAnimation_h
#define ParticlesAnimation_h

#include "Action.h"
#include "particle/ParticleAnimationInit.h"

#include "particle/ParticleVelocityInit.h"

namespace Supernova{
    
    class ParticlesAnimation: public Action{

    protected:

        std::vector<ParticleAnimationInit*> particlesInit;

        float newParticlesCount;
        
    public:
        ParticlesAnimation();
        virtual ~ParticlesAnimation();

        void addInit(ParticleAnimationInit* particleInit);
        void removeInit(ParticleAnimationInit* particleInit);
        
        virtual void play();
        virtual void stop();
        virtual void reset();
        
        virtual void step();
    };
    
}

#endif /* ParticlesAnimation_h */
