//
// (c) 2023 Eduardo Doria.
//

#ifndef TILEMAP_H
#define TILEMAP_H

#include "Mesh.h"

namespace Supernova{

    class Tilemap: public Mesh{

    public:
        Tilemap(Scene* scene);
        virtual ~Tilemap();

    };

}

#endif //TERRAIN_H