//
// (c) 2019 Eduardo Doria.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Mesh.h"

namespace Supernova{

    class Terrain: public Mesh {
    private:

        InterleavedBuffer buffer;
        TextureData* heightmap_data;
        std::string heightmap_path;

        bool loadTerrainFromImage(std::string path);
        void computeNormals();
        Vector3 terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3);
        bool terrainScale(float min, float max);
        bool terrainCreate(float xOffset, float yOffset, float zOffset);

    protected:

        int terrainGridWidth;
        int terrainGridLength;
        std::vector<float> terrainHeights;
        std::vector<Vector3> terrainNormals;

    public:

        Terrain();
        Terrain(std::string heightmap);
        virtual ~Terrain();

        const std::string &getHeightmap() const;
        void setHeightmap(const std::string &heightmap);

        bool load();
    };

}


#endif //TERRAIN_H
