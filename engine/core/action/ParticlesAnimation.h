#ifndef ParticlesAnimation_h
#define ParticlesAnimation_h

#include "action/Action.h"
#include "action/ParticleInit.h"

namespace Supernova{
    
    class ParticlesAnimation: public Action{

    protected:

        std::vector<ParticleInit*> particlesInit;

        float newParticlesCount;
        bool emitter;

        bool initOwned;
        
    public:
        ParticlesAnimation();
        virtual ~ParticlesAnimation();

        void addInit(ParticleInit* particleInit);
        void removeInit(ParticleInit* particleInit);
        void deleteInits();
        void setInitOwned(bool initOwned);
        
        virtual bool run();
        virtual bool pause();
        virtual bool stop();

        void startEmitter();
        void stopEmitter();

        bool isEmitting();
        
        virtual bool step();
    };
    
}

#endif /* ParticlesAnimation_h */
