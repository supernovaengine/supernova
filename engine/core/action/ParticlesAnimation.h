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
        bool emitter;
        
    public:
        ParticlesAnimation();
        virtual ~ParticlesAnimation();

        void addInit(ParticleAnimationInit* particleInit);
        void removeInit(ParticleAnimationInit* particleInit);
        
        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        bool stopAll();
        
        virtual bool step();
    };
    
}

#endif /* ParticlesAnimation_h */
