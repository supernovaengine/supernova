//
// (c) 2022 Eduardo Doria.
//

#ifndef IMAGE_H
#define IMAGE_H

#include "UIObject.h"

namespace Supernova{
    class Image: public UIObject{

    protected:
        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        Image(Scene* scene);

        void setPatchMargin(int margin);

        void setPatchMarginBottom(int marginBottom);
        void setPatchMarginLeft(int marginLeft);
        void setPatchMarginRight(int marginRight);
        void setPatchMarginTop(int marginTop);

        int getPatchMarginBottom() const;
        int getPatchMarginLeft() const;
        int getPatchMarginRight() const;
        int getPatchMarginTop() const;

        void setTexture(std::string path);
        void setTexture(FramebufferRender* framebuffer);

        void setColor(Vector4 color);
        void setColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getColor() const;
    };
}

#endif //IMAGE_H