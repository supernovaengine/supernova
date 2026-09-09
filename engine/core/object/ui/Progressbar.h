// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "Image.h"

namespace doriax{
    class DORIAX_API Progressbar: public Image{
    private:
        void ensureFill();

    public:
        Progressbar(Scene* scene);
        Progressbar(Scene* scene, Entity entity);
        virtual ~Progressbar();

        bool hasFill() const;
        Image getFillObject() const;

        void setType(ProgressbarType type);
        ProgressbarType getType() const;

        void setValue(float value);
        float getValue() const;

        void setFillTexture(const std::string& path);
        void setFillTexture(Framebuffer* framebuffer);

        void setFillColor(Vector4 color);
        void setFillColor(const float red, const float green, const float blue, const float alpha);
        void setFillColor(const float red, const float green, const float blue);
        void setFillAlpha(const float alpha);
        Vector4 getFillColor() const;
        float getFillAlpha() const;

        void setFillPatchMargin(int margin);
        void setFillPatchMargin(int marginLeft, int marginRight, int marginTop, int marginBottom);

        void setFillMargin(int margin);
        void setFillMargin(int marginLeft, int marginRight, int marginTop, int marginBottom);
    };
}

#endif //PROGRESSBAR_H
