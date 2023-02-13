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

        bool load();
        bool createTerrain();

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
        void setColor(const float red, const float green, const float blue, const float alpha);
        void setColor(const float red, const float green, const float blue);
        Vector4 getColor() const;

        void setSize(float size);
        float getSize() const;

        void setMaxHeight(float maxHeight);
        float getMaxHeight() const;

        void setResolution(int resolution);
        int getResolution() const;

        void setTextureBaseTiles(int textureBaseTiles);
        int getTextureBaseTiles() const;

        void setTextureDetailTiles(int textureDetailTiles);
        int getTextureDetailTiles() const;

        void setRootGridSize(int rootGridSize);
        int getRootGridSize() const;

        void setLevels(int levels);
        int getLevels() const;
    };

}

#endif //TERRAIN_H