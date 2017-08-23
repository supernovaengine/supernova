#ifndef ParticlesAnimation_h
#define ParticlesAnimation_h

#include "Action.h"

namespace Supernova{
    
    class ParticlesAnimation: public Action{
        
    public:
        ParticlesAnimation();
        virtual ~ParticlesAnimation();
        
        virtual void play();
        virtual void stop();
        virtual void reset();
        
        virtual void step();
    };
    
}

#endif /* ParticlesAnimation_h */
