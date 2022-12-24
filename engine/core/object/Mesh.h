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

        void setTexture(std::string path);
        void setTexture(Framebuffer* framebuffer);
        
        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor() const;

        void createPlane(float width, float depth);
        void createCube(float width, float height, float depth);
        void createSphere(float radius=1, float slices=36, float stacks=18);
    };
}

#endif //MESH_H