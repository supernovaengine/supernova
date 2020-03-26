//
// (c) 2020 Eduardo Doria.
//

#ifndef TERRAINNODE_H
#define TERRAINNODE_H

#include "Submesh.h"

namespace Supernova{

    class TerrainNode: public Submesh {
    private:
        Vector2 offset;
        float scale;

        TerrainNode* childs[4];

    public:
        TerrainNode(float x, float y, float scale, int lodDepth, std::vector<Submesh*>* submeshes);
        virtual ~TerrainNode();

        const Vector2 &getOffset() const;
        void setOffset(const Vector2 &offset);

        float getScale() const;
        void setScale(float scale);

        virtual void setIndices(std::string bufferName, size_t size, size_t offset = 0, DataType type = UNSIGNED_INT);

        virtual bool renderLoad(bool shadow);
    };

}


#endif //TERRAINNODE_H
