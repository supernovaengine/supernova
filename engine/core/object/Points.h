//
// (c) 2024 Eduardo Doria.
//

#ifndef POINTS_H
#define POINTS_H

#include "Object.h"
#include "buffer/ExternalBuffer.h"

namespace Supernova{

    class Points: public Object{
    public:
        Points(Scene* scene);
        virtual ~Points();

        bool load();

        void setMaxPoints(unsigned int maxPoints);
        unsigned int getMaxPoints() const;

        void addPoint(Vector3 position);
        void addPoint(Vector3 position, Vector4 color);
        void addPoint(Vector3 position, Vector4 color, float size, float rotation);
        void addPoint(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect);
        void addPoint(float x, float y, float z);

        void clearPoints();

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

#endif //POINTS_H
