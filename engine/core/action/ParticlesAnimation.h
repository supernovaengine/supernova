#ifndef ParticlesAnimation_h
#define ParticlesAnimation_h

#include "action/Action.h"
#include "action/ParticleInit.h"
#include "action/ParticleMod.h"

namespace Supernova{
    
    class ParticlesAnimation: public Action{

    protected:

        std::vector<ParticleInit*> particlesInit;
        std::vector<ParticleMod*> particlesMod;

        float newParticlesCount;
        bool emitter;

        bool initOwned;
        bool modOwned;

    public:
        ParticlesAnimation();
        virtual ~ParticlesAnimation();

        void addInit(ParticleInit* particleInit);
        void removeInit(ParticleInit* particleInit);
        void deleteInits();
        void setInitOwned(bool initOwned);

        void addMod(ParticleMod* particleMod);
        void removeMod(ParticleMod* particleMod);
        void deleteMods();
        void setModOwned(bool modOwned);
        
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
