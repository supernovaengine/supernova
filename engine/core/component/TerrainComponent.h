#ifndef TERRAIN_COMPONENT_H
#define TERRAIN_COMPONENT_H

#define MAX_TERRAINGRID 16
#define MAX_TERRAINNODES 5460
// to get best terrainnodes number, use this formula:
// ( 4⁰ + 4¹ + 4² + ... + 4^(levels-1) ) * rootGridSize²

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"
#include "Supernova.h"

namespace Supernova{

    struct TerrainNodeIndex{
        unsigned int indexCount;
        unsigned int indexOffset;
    };

    struct TerrainNode{
        Attribute indexAttribute;

        Vector2 position = Vector2(0, 0);
        float size = 0;
        //Terrain* terrain;

        size_t childs[4];
        bool hasChilds = false;

        float currentRange = 0;
        int resolution = 0;

        float maxHeight = 0;
        float minHeight = 0;
        
        float visible = false;
    };

    struct TerrainComponent{
        InterleavedBuffer buffer;
        IndexBuffer indices;

        Texture heightMap;

        TerrainNodeIndex fullResNode = {0, 0};
        TerrainNodeIndex halfResNode = {0, 0};

        bool autoSetRanges = true;
        bool heightMapLoaded = false;

        Vector2 offset;
        std::vector<float> ranges;

        TerrainNode nodes[MAX_TERRAINNODES];
        unsigned int numNodes = 0;

        size_t grid[MAX_TERRAINGRID]; //root nodes

        float terrainSize = 200;
        float maxHeight = 10;
        int rootGridSize = 2;
        int levels = 6;
        int resolution = 32;

        int textureBaseTiles = 1;
        int textureDetailTiles = 20;

        bool needUpdateTerrain = true;
    };
    
}

#endif //TERRAIN_COMPONENT_H