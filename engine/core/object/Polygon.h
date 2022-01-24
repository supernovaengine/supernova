//
// (c) 2022 Eduardo Doria.
//

#ifndef POLYGON_H
#define POLYGON_H

#include "Mesh.h"

namespace Supernova{
    class Polygon: public Mesh{

    public:
        Polygon(Scene* scene);

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);

        int getWidth();
        int getHeight();

        void setFlipY(bool flipY);
    };
}

#endif //POLYGON_H