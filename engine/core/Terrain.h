//
// (c) 2019 Eduardo Doria.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Mesh.h"

namespace Supernova{

    class Terrain: public Mesh {
        struct Tile{

        };

    private:

        InterleavedBuffer buffer;
        IndexBuffer indices;
        std::string heightmap_path;

        Vector2 offset;

        void createPlaneTile(int index, int widthSegments, int heightSegments);

    protected:

    public:

        Terrain();
        Terrain(std::string heightmap);
        virtual ~Terrain();

        const std::string &getHeightmap() const;
        void setHeightmap(const std::string &heightmap);

        virtual bool shadowLoad();
        virtual bool load();
    };

}


#endif //TERRAIN_H
