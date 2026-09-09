// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef OCCLUDER2D_H
#define OCCLUDER2D_H

#include "Object.h"

namespace doriax{

    class DORIAX_API Occluder2D: public Object{
    public:
        Occluder2D(Scene* scene);
        Occluder2D(Scene* scene, Entity entity);
        virtual ~Occluder2D();

        void setShape(Occluder2DShape shape);
        Occluder2DShape getShape() const;

        void addVertex(Vector2 vertex);
        void addVertex(float x, float y);
        void clearVertices();
        unsigned int getVertexCount() const;

        void setClosed(bool closed);
        bool isClosed() const;

        void setEnabled(bool enabled);
        bool isEnabled() const;
    };

}

#endif //OCCLUDER2D_H
