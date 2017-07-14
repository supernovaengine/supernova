#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

#include "GLES2Header.h"
#include <unordered_map>

namespace Supernova {
    class GLES2Object: public ObjectRender{
        
        struct bufferData{
            GLuint buffer;
            GLuint size;
        };
        
    private:
        
        GLenum usageBuffer;
        
        std::unordered_map<int, bufferData> vertexAttributes;
        bufferData indexAttribute;
        std::unordered_map<int, GLuint> properties;

    public:
        GLES2Object();
        virtual ~GLES2Object();

        virtual void loadVertexAttribute(int type, unsigned int elements, unsigned long size, void* data);
        virtual void loadIndex(unsigned long size, void* data);
        
        virtual void setProperty(int type, int datatype, unsigned long size, void* data);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
