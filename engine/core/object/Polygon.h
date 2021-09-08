//
// (c) 2021 Eduardo Doria.
//

#ifndef POLYGON_H
#define POLYGON_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

#include "Object.h"

namespace Supernova{
    class Polygon: public Object{

    protected:
        InterleavedBuffer buffer;
        IndexBuffer indices;

        void generateTexcoords();

    public:
        Polygon(Scene* scene);

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();

        void setTexture(std::string path);

        void setTextureRect(float x, float y, float width, float height);
        void setTextureRect(Rect textureRect);
        Rect getTextureRect();

    };
}

#endif //POLYGON_H