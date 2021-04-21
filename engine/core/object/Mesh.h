//
// (c) 2021 Eduardo Doria.
//

#ifndef MESH_H
#define MESH_H

#include "Object.h"
#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

namespace Supernova{

    class Mesh: public Object{
    protected:
        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        Mesh(Scene* scene);
        virtual ~Mesh();

        void addVertex(Vector3 vertex);
        void addVertex(float x, float y);

        void setTexture(std::string path);
        
        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();

        void addSubmeshAttribute(Submesh& submesh, std::string bufferName, AttributeType attribute, unsigned int elements, AttributeDataType dataType, size_t size, size_t offset, bool normalized);

        void generateTexcoords();
    };
}

#endif //MESH_H