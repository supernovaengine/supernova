//
// (c) 2024 Eduardo Doria.
//

#ifndef POINTPARTICLES_H
#define POINTPARTICLES_H

#include "Object.h"
#include "buffer/ExternalBuffer.h"

namespace Supernova{

    class PointParticles: public Object{
    public:
        PointParticles(Scene* scene);
        virtual ~PointParticles();

        bool load();

        void setMaxParticles(unsigned int maxParticles);
        unsigned int getMaxParticles() const;

        void addParticle(Vector3 position);
        void addParticle(Vector3 position, Vector4 color);
        void addParticle(Vector3 position, Vector4 color, float size, float rotation);
        void addParticle(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect);
        void addParticle(float x, float y, float z);

        void addSpriteFrame(int id, std::string name, Rect rect);
        void addSpriteFrame(std::string name, float x, float y, float width, float height);
        void addSpriteFrame(float x, float y, float width, float height);
        void addSpriteFrame(Rect rect);
        void removeSpriteFrame(int id);
        void removeSpriteFrame(std::string name);

        void setTexture(std::string path);
        void setTexture(Framebuffer* framebuffer);
    };
}

#endif //POINTPARTICLES_H