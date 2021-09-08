//
// (c) 2021 Eduardo Doria.
//

#ifndef SPRITE_H
#define SPRITE_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

#include "Polygon.h"

namespace Supernova{
    class Sprite: public Polygon{

    public:
        Sprite(Scene* scene);

        void addFrame(int id, std::string name, Rect rect);
        void addFrame(std::string name, float x, float y, float width, float height);
        void addFrame(float x, float y, float width, float height);
        void addFrame(Rect rect);
        void removeFrame(int id);
        void removeFrame(std::string name);

        void setFrame(int id);
        void setFrame(std::string name);
    };
}

#endif //SPRITE_H