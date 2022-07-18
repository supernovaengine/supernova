//
// (c) 2022 Eduardo Doria.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Object.h"

namespace Supernova{

    class Terrain: public Object{

    public:
        Terrain(Scene* scene);
        virtual ~Terrain();

        void setHeightMap(std::string path);
        void setHeightMap(FramebufferRender* framebuffer);

        void setTexture(std::string path);
        void setTexture(FramebufferRender* framebuffer);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();
    };

}

#endif //TERRAIN_H