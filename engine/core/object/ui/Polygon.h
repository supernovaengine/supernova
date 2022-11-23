//
// (c) 2022 Eduardo Doria.
//

#ifndef POLYGON_H
#define POLYGON_H

#include "UILayout.h"

namespace Supernova{
    class Polygon: public UILayout{

    public:
        Polygon(Scene* scene);

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);

        void setTexture(std::string path);
        void setTexture(FramebufferRender* framebuffer);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor() const;

        void setFlipY(bool flipY);
        bool isFlipY() const;
    };
}

#endif //POLYGON_H