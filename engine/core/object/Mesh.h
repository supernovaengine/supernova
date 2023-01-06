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
    public:
        Mesh(Scene* scene);
        virtual ~Mesh();

        void setTexture(std::string path);
        void setTexture(Framebuffer* framebuffer);
        
        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor() const;

        void createPlane(float width, float depth);
        void createPlane(float width, float depth, unsigned int tiles);
        
        void createBox(float width, float height, float depth);
        void createBox(float width, float height, float depth, unsigned int tiles);

        void createSphere(float radius);
        void createSphere(float radius, unsigned int slices, unsigned int stacks);

        void createCylinder(float radius, float height);
        void createCylinder(float baseRadius, float topRadius, float height);
        void createCylinder(float radius, float height, unsigned int slices, unsigned int stacks);
        void createCylinder(float baseRadius, float topRadius, float height, unsigned int slices, unsigned int stacks);

        void createTorus(float radius, float ringRadius);
        void createTorus(float radius, float ringRadius, unsigned int sides, unsigned int rings);
    };
}

#endif //MESH_H