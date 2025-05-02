//
// (c) 2024 Eduardo Doria.
//

#ifndef IMAGE_H
#define IMAGE_H

#include "UILayout.h"

namespace Supernova{
    class SUPERNOVA_API Image: public UILayout{
    public:
        Image(Scene* scene);
        Image(Scene* scene, Entity entity);

        bool createImage();
        bool load();

        void setPatchMargin(unsigned int margin);
        void setPatchMargin(unsigned int marginLeft, unsigned int marginRight, unsigned int marginTop, unsigned int marginBottom);

        void setPatchMarginBottom(unsigned int marginBottom);
        void setPatchMarginLeft(unsigned int marginLeft);
        void setPatchMarginRight(unsigned int marginRight);
        void setPatchMarginTop(unsigned int marginTop);

        unsigned int getPatchMarginBottom() const;
        unsigned int getPatchMarginLeft() const;
        unsigned int getPatchMarginRight() const;
        unsigned int getPatchMarginTop() const;

        void setTexture(const std::string& path);
        void setTexture(Framebuffer* framebuffer);

        void setTextureCutFactor(float textureCutFactor);
        float getTextureCutFactor() const;

        void setColor(Vector4 color);
        void setColor(const float red, const float green, const float blue, const float alpha);
        void setColor(const float red, const float green, const float blue);
        void setAlpha(const float alpha);
        Vector4 getColor() const;
        float getAlpha() const;

        void setFlipY(bool flipY);
        bool isFlipY() const;

        AABB getAABB() const;
        AABB getWorldAABB() const;
    };
}

#endif //IMAGE_H