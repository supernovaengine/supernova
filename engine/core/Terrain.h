//
// (c) 2019 Eduardo Doria.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Mesh.h"
#include "TerrainNode.h"

namespace Supernova{

    class Terrain: public Mesh {
        friend class TerrainNode;

    private:

        InterleavedBuffer buffer;
        IndexBuffer indices;

        Texture* heightData;

        unsigned int bufferIndexCount;

        Vector2 offset;
        std::vector<float> ranges;
        std::vector<TerrainNode*> grid;
        float rootNodeSize;

    private:

        float worldWidth;
        int levels;
        int resolution;

        void createPlaneNodeBuffer(int width, int height, int widthSegments, int heightSegments);
        TerrainNode* createNode(float x, float y, float scale, int lodDepth);

    protected:

        void updateNodes();

        virtual void updateModelMatrix();
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);

    public:

        Terrain();
        Terrain(std::string heightMapPath);
        virtual ~Terrain();

        Texture* getHeightmap();
        void setHeightmap(std::string heightMapPath);

        virtual bool renderLoad(bool shadow);
        virtual bool load();
    };

}


#endif //TERRAIN_H
