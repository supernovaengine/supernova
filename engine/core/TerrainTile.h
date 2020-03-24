//
// (c) 2020 Eduardo Doria.
//

#ifndef TERRAINTILE_H
#define TERRAINTILE_H

#include "Submesh.h"

namespace Supernova{

    class TerrainTile: public Submesh {
    private:
        Vector2 offset;
        float scale;

        TerrainTile* childs[4];

    public:
        TerrainTile(float x, float y, float scale, int lodDepth, std::vector<Submesh*>* submeshes);
        virtual ~TerrainTile();

        const Vector2 &getOffset() const;
        void setOffset(const Vector2 &offset);

        float getScale() const;
        void setScale(float scale);

        virtual void setIndices(std::string bufferName, size_t size, size_t offset = 0, DataType type = UNSIGNED_INT);

        virtual bool renderLoad(bool shadow);
    };

}


#endif //TERRAINTILE_H
