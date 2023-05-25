#ifndef TERRAIN_COMPONENT_H
#define TERRAIN_COMPONENT_H

#define MAX_TERRAINGRID 16

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"
#include "texture/Material.h"
#include "Supernova.h"

namespace Supernova{

    struct TerrainNodeIndex{
        unsigned int indexCount;
        unsigned int indexOffset;
    };

    struct TerrainNode{
        Attribute indexAttribute;

        //-----u_vs_terrainNodeParams
        Vector2 position = Vector2(0, 0);
        float size = 0;
        float currentRange = 0;
        float resolution = 0; //int
        uint8_t _pad_20[12];
        //-----

        size_t childs[4];
        bool hasChilds = false;

        float maxHeight = 0;
        float minHeight = 0;
        
        float visible = false;
    };

    struct TerrainComponent{
        bool loaded = false;
        bool loadCalled = false;

        Material material;

        InterleavedBuffer buffer;
        IndexBuffer indices;

        ObjectRender render;
        ObjectRender depthRender;

        std::shared_ptr<ShaderRender> shader;
        std::shared_ptr<ShaderRender> depthShader;

        std::string shaderProperties;
        std::string depthShaderProperties;

        int slotVSParams = -1;
        int slotFSParams = -1;
        int slotFSLighting = -1;
        int slotFSFog = -1;
        int slotVSShadows = -1;
        int slotFSShadows = -1;
        int slotVSTerrain = -1;
        int slotVSTerrainNode = -1;

        int slotVSDepthParams = -1;

        bool castShadows = true;

        Texture heightMap;
        Texture blendMap;
        Texture textureDetailRed;
        Texture textureDetailGreen;
        Texture textureDetailBlue;

        TerrainNodeIndex fullResNode = {0, 0};
        TerrainNodeIndex halfResNode = {0, 0};

        bool autoSetRanges = true;
        bool heightMapLoaded = false;

        Vector2 offset;
        std::vector<float> ranges;

        //using std::vector to avoid chkstk.asm stack overflow error in Windows
        std::vector<TerrainNode> nodes;
        unsigned int numNodes = 0;

        size_t grid[MAX_TERRAINGRID]; //root nodes

        //-----u_vs_terrainParams
        Vector3 eyePos;
        float terrainSize = 200;
        float maxHeight = 10;
        float resolution = 32; //int
        float textureBaseTiles = 1; //int
        float textureDetailTiles = 20; //int
        //-----

        int rootGridSize = 2;
        int levels = 6;

        bool needUpdateTerrain = true;
        bool needUpdateTexture = false;
        bool needReload = false;
    };
    
}

#endif //TERRAIN_COMPONENT_H
