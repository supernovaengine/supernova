#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

#include "GLES2Header.h"
#include <unordered_map>

namespace Supernova {
    class GLES2Object: public ObjectRender{
        
        struct bufferProperty{
            GLuint buffer;
            GLuint size;
        };
        
    private:
        
        GLenum usageBuffer;
        std::unordered_map<int, bufferProperty> bufferProperties;

    public:
        GLES2Object();
        virtual ~GLES2Object();

        virtual void loadVertexProperty(int type, unsigned int count, unsigned long size, void* data);
        virtual void loadIndex(int type, unsigned long size, void* data);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
