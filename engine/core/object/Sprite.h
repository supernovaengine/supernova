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

    };
}

#endif //SPRITE_H