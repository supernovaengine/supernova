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
        void setHeightMap(Framebuffer* framebuffer);

        void setBlendMap(std::string path);
        void setBlendMap(Framebuffer* framebuffer);

        void setTextureDetailRed(std::string path);
        void setTextureDetailGreen(std::string path);
        void setTextureDetailBlue(std::string path);

        void setTexture(std::string path);
        void setTexture(Framebuffer* framebuffer);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor() const;
    };

}

#endif //TERRAIN_H