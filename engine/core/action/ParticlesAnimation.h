#ifndef ParticlesAnimation_h
#define ParticlesAnimation_h

#include "action/Action.h"
#include "action/particle/ParticleInit.h"

#include "particle/ParticleVelocityInit.h"

namespace Supernova{
    
    class ParticlesAnimation: public Action{

    protected:

        std::vector<ParticleInit*> particlesInit;

        float newParticlesCount;
        bool emitter;
        
    public:
        ParticlesAnimation();
        virtual ~ParticlesAnimation();

        void addInit(ParticleInit* particleInit);
        void removeInit(ParticleInit* particleInit);
        
        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        bool stopAll();
        
        virtual bool step();
    };
    
}

#endif /* ParticlesAnimation_h */
