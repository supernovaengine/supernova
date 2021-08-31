//
// (c) 2021 Eduardo Doria.
//

#ifndef SPRITE_H
#define SPRITE_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

#include "Object.h"
#include "Scene.h"
#include "Entity.h"

namespace Supernova{
    class Sprite: public Object{

    protected:
        //Entity entity;
        //Scene* scene;

        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        Sprite(Scene* scene);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();

        void setTexture(std::string path);

    };
}

#endif //SPRITE_H