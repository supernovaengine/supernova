//
// (c) 2021 Eduardo Doria.
//

#ifndef IMAGE_H
#define IMAGE_H

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
        void setTexture(FramebufferRender* framebuffer);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();

        int getWidth();
        int getHeight();
    };
}

#endif //IMAGE_H