//
// (c) 2024 Eduardo Doria.
//

#ifndef PARTICLES_H
#define PARTICLES_H

#include "Model.h"

namespace Supernova{

    class Particles: public Model{
    public:
        Particles(Scene* scene);
        virtual ~Particles();

        bool load();

        void setMaxParticles(unsigned int maxParticles);
        unsigned int getMaxParticles() const;

        void addParticle(Vector3 position);
        void addParticle(float x, float y, float z);

        void setTexture(std::string path);
        void setTexture(Framebuffer* framebuffer);
    };
}

#endif //POINTPARTICLES_H