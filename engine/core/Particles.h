#ifndef Particles_h
#define Particles_h

#include "Points.h"

namespace Supernova {

    class Particles: public Points{

    private:
        int lastUsedParticle;
        
        void createParticles();
        
    protected:

        struct ParticleData{
            int life;
        };

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
