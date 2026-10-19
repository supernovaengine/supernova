// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Mesh.h"
#include "component/TerrainComponent.h"

namespace doriax{

    class DORIAX_API Terrain: public Mesh{

    public:
        Terrain(Scene* scene);
        Terrain(Scene* scene, Entity entity);
        virtual ~Terrain();

        bool createTerrain();

        void setHeightMap(const std::string& path);
        void setHeightMap(Framebuffer* framebuffer);

        void setBlendMap(const std::string& path);
        void setBlendMap(Framebuffer* framebuffer);

        void setBlendMap(unsigned int index, const std::string& path);

        // Only the layer color: a PBR layer keeps the rest of its surface
        void setTextureLayer(unsigned int index, const std::string& path);

        void setTextureDetailRed(const std::string& path);
        void setTextureDetailGreen(const std::string& path);
        void setTextureDetailBlue(const std::string& path);

        void setSurfaceLayer(unsigned int index, const TerrainSurfaceLayer& layer);
        TerrainSurfaceLayer getSurfaceLayer(unsigned int index) const;
        // Copies what a terrain layer can render, and turns the layer into a PBR one
        void setLayerFromMaterial(unsigned int index, const Material& material);
        void removeSurfaceLayer(unsigned int index);
        unsigned int getNumLayers() const;

        void setSize(float size);
        float getSize() const;

        void setMaxHeight(float maxHeight);
        float getMaxHeight() const;

        // quality setting: scales the draw distances of every foliage layer (1 = as authored)
        void setFoliageDistanceScale(float scale);
        float getFoliageDistanceScale() const;

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