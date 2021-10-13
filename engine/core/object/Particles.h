//
// (c) 2021 Eduardo Doria.
//

#ifndef PARTICLES_H
#define PARTICLES_H

#include "Object.h"
#include "buffer/ExternalBuffer.h"

namespace Supernova{

    class Particles: public Object{
    protected:
        ExternalBuffer buffer;

    public:
        Particles(Scene* scene);
        virtual ~Particles();

        void addParticle(Vector3 position);
        void addParticle(Vector3 position, Vector4 color);
        void addParticle(Vector3 position, Vector4 color, float size, float rotation);
        void addParticle(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect);
        void addParticle(float x, float y, float z);

        void setTexture(std::string path);
    };
}

#endif //PARTICLES_H