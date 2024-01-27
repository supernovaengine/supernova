//
// (c) 2023 Eduardo Doria.
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

        bool load();

        void setTexture(std::string path);
        void setTexture(Framebuffer* framebuffer);
        
        void setColor(Vector4 color);
        void setColor(const float red, const float green, const float blue, const float alpha);
        void setColor(const float red, const float green, const float blue);
        void setAlpha(const float alpha);
        Vector4 getColor() const;
        float getAlpha() const;

        void setPrimitiveType(PrimitiveType primitiveType);
        PrimitiveType getPrimitiveType() const;

        void setPrimitiveType(unsigned int submesh, PrimitiveType primitiveType);
        PrimitiveType getPrimitiveType(unsigned int submesh) const;

        Material& getMaterial(unsigned int submesh = 0);
    };
}

#endif //MESH_H