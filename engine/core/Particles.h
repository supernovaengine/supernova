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
            float life;
            Vector3 velocity;
            Vector3 acceleration;
        };

        std::vector<ParticleData> particles;
        int maxParticles;
        int minRate; //per second
        int maxRate;
        
    public:
        Particles();
        Particles(int numParticles);
        virtual ~Particles();
        
        void setMaxParticles(int maxParticles);
        int getMaxParticles();
        
        virtual void addPoint();
        virtual void addPoint(Vector3 position);
        virtual void clearPoints();
        
        void addParticle();
        void addParticle(Vector3 position);
        void clearParticles();
        
        void setParticlePosition(int particle, Vector3 position);
        void setParticlePosition(int particle, float x, float y, float z);
        void setParticleSize(int particle, float size);
        void setParticleColor(int particle, Vector4 color);
        void setParticleColor(int particle, float red, float green, float blue, float alpha);
        void setParticleSprite(int particle, int index);
        void setParticleVisible(int particle, bool visible);
        void setParticleLife(int particle, float life);
        void setParticleVelocity(int particle, Vector3 velocity);
        void setParticleAcceleration(int particle, Vector3 acceleration);
        
        Vector3 getParticlePosition(int particle);
        float getParticleSize(int particle);
        Vector4 getParticleColor(int particle);
        float getParticleLife(int particle);
        Vector3 getParticleVelocity(int particle);
        Vector3 getParticleAcceleration(int particle);

        void setRate(int minRate);
        void setRate(int minRate, int maxRate);

        int getMinRate();
        int getMaxRate();
        
        int findUnusedParticle();

        void updateParticles();

        virtual bool load();
        virtual bool draw();
    };

}


#endif /* Particles_h */
