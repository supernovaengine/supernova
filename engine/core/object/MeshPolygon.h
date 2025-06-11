//
// (c) 2025 Eduardo Doria.
//

#ifndef MESHPOLYGON_H
#define MESHPOLYGON_H

#include "Mesh.h"

namespace Supernova{
    class SUPERNOVA_API MeshPolygon: public Mesh{

    public:
        MeshPolygon(Scene* scene);
        MeshPolygon(Scene* scene, Entity entity);
        virtual ~MeshPolygon();

        bool createPolygon();

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);

        void clearVertices();

        unsigned int getWidth();
        unsigned int getHeight();

        void setFlipY(bool flipY);
        bool isFlipY() const;
    };
}

#endif //MESHPOLYGON_H