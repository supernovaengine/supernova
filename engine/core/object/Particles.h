//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLES_H
#define PARTICLES_H

#include "Object.h"
#include "buffer/InterleavedBuffer.h"

namespace Supernova{

    class Particles: public Object{
    protected:
        InterleavedBuffer buffer;

    public:
        Particles(Scene* scene);
        virtual ~Particles();

        void addParticle(Vector3 vertex);
        void addParticle(Vector3 vertex, Vector4 color);
        void addParticle(float x, float y, float z);

        void setTexture(std::string path);
    };
}

#endif //PARTICLES_H