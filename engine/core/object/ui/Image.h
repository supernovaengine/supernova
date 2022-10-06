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
        void setWidth(int width);
        void setHeight(int height);

        int getWidth() const;
        int getHeight() const;

        void setMargin(int margin);

        void setMarginBottom(int marginBottom);
        void setMarginLeft(int marginLeft);
        void setMarginRight(int marginRight);
        void setMarginTop(int marginTop);

        int getMarginBottom() const;
        int getMarginLeft() const;
        int getMarginRight() const;
        int getMarginTop() const;

        void setTexture(std::string path);
        void setTexture(FramebufferRender* framebuffer);

        void setColor(Vector4 color);
        void setColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getColor() const;
    };
}

#endif //IMAGE_H