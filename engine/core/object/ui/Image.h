//
// (c) 2021 Eduardo Doria.
//

#ifndef IMAGE_H
#define IMAGE_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

#include "Object.h"

namespace Supernova{
    class Image: public Object{

    protected:
        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        Image(Scene* scene);

        void setSize(int width, int height);
        void setMargin(int margin);

        void setTexture(std::string path);

        int getWidth();
        int getHeight();
    };
}

#endif //IMAGE_H