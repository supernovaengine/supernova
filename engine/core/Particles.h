#ifndef Particles_h
#define Particles_h

#include "Points.h"

namespace Supernova {

    class Particles: public Points{
        
        struct ParticleData{
            int life;
        };
        
    private:
        int lastUsedParticle;
        
        void createParticles();
        
    protected:
        std::vector<ParticleData> particlesData;
        int numParticles;
        
    public:
        Particles();
        Particles(int numParticles);
        virtual ~Particles();
        
        void setNumParticles(int numParticles);
        int getNumParticles();
        
        int findUnusedParticle();
        
        virtual bool load();
    };

}


#endif /* Particles_h */
