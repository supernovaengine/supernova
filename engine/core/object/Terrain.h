//
// (c) 2022 Eduardo Doria.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "Object.h"

namespace Supernova{

    class Terrain: public Object{

    public:
        Terrain(Scene* scene);
        virtual ~Terrain();

        void setHeightMap(std::string path);
        void setHeightMap(FramebufferRender* framebuffer);

    };

}

#endif //TERRAIN_H