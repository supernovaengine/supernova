//
// (c) 2022 Eduardo Doria.
//

#ifndef POLYGON_H
#define POLYGON_H

#include "Mesh.h"

namespace Supernova{
    class Polygon: public Mesh{

    protected:
        void generateTexcoords();

    public:
        Polygon(Scene* scene);

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);
    };
}

#endif //POLYGON_H