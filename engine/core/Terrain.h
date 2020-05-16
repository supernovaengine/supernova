//
// (c) 2019 Eduardo Doria.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Mesh.h"
#include "TerrainNode.h"

namespace Supernova{

    enum BlendMapColor{
        RED = 0,
        GREEN = 1,
        BLUE = 2
    };

    class Terrain: public Mesh {
        friend class TerrainNode;

    private:

        InterleavedBuffer buffer;
        IndexBuffer indices;

        Texture* heightMap;

        Texture* blendMap;
        std::vector<Texture*> textureDetails;
        std::vector<int> blendMapColorIndex;

        struct NodeIndex{
            unsigned int indexCount;
            unsigned int indexOffset;
        };

        NodeIndex fullResNode;
        NodeIndex halfResNode;

        bool autoSetRanges;
        bool heightMapLoaded;

        Vector2 offset;
        std::vector<float> ranges;

        std::vector<TerrainNode*> grid;

        float terrainSize;
        float maxHeight;
        int rootGridSize;
        int levels;
        int resolution;

        int textureBaseTiles;
        int textureDetailTiles;

        NodeIndex createPlaneNodeBuffer(int width, int height, int widthSegments, int heightSegments);
        TerrainNode* createNode(float x, float y, float scale, int lodDepth);

    protected:

        void updateNodes();

        virtual void updateModelMatrix();
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

    public:

        Terrain();
        Terrain(std::string heightMapPath);
        virtual ~Terrain();

        Texture* getHeightMap();
        void setHeightMap(std::string heightMapPath);

        Texture* getBlendMap();
        void setBlendMap(std::string blendMapPath);

        void addTextureDetail(std::string heightMapPath, BlendMapColor blendMapColor);
        bool removeTextureDetail(BlendMapColor blendMapColor);

        const std::vector<float> &getRanges() const;
        void setRanges(const std::vector<float> &ranges);

        int getTextureBaseTiles() const;
        void setTextureBaseTiles(int textureBaseTiles);

        int getTextureDetailTiles() const;
        void setTextureDetailTiles(int textureDetailTiles);

        float getHeight(float x, float y);
        float maxHeightArea(float x, float z, float w, float h);
        float minHeightArea(float x, float z, float w, float h);

        virtual bool renderLoad(bool shadow);
        virtual bool load();
    };

}


#endif //TERRAIN_H
