//
// (c) 2022 Eduardo Doria.
//

#ifndef MESHPOLYGON_H
#define MESHPOLYGON_H

#include "Mesh.h"

namespace Supernova{
    class MeshPolygon: public Mesh{

    public:
        MeshPolygon(Scene* scene);

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);

        int getWidth();
        int getHeight();

        void setFlipY(bool flipY);
    };
}

#endif //MESHPOLYGON_H