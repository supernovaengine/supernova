//
// (c) 2020 Eduardo Doria.
//

#ifndef TERRAINNODE_H
#define TERRAINNODE_H

#include "Submesh.h"
#include "math/AlignedBox.h"


namespace Supernova{

    class Terrain;

    class TerrainNode: public Submesh {

    private:
        Vector2 position;
        float size;
        Terrain* terrain;

        TerrainNode* childs[4];

        float currentRange;
        int resolution;

        float maxHeight;
        float minHeight;

        bool inSphere(float radius, const AlignedBox& box);
        bool inFrustum(const AlignedBox& box);

    public:
        TerrainNode(float x, float y, float size, int lodDepth, Terrain* submeshes);
        virtual ~TerrainNode();

        const Vector2 &getPosition() const;
        void setPosition(const Vector2 &position);

        float getSize() const;
        void setSize(float size);

        AlignedBox getAlignedBox();

        bool LODSelect(std::vector<float> &ranges, int lodLevel);

        virtual void setIndices(std::string bufferName, size_t size, size_t offset = 0, DataType type = UNSIGNED_INT);

        virtual bool renderLoad(bool shadow);
    };

}


#endif //TERRAINNODE_H
