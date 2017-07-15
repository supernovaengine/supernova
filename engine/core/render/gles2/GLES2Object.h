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
            GLint handle;
        };
        
    private:
        
        GLenum usageBuffer;
        
        std::unordered_map<int, bufferData> attributeBuffers;
        bufferData indexBuffer;
        std::unordered_map<int, GLuint> propertyBuffers;
        
    protected:
        
        void loadVertexAttribute(int type, attributeData att);
        void loadIndex(attributeData att);
        
        void useProperty(int type, propertyData prop);

    public:
        GLES2Object();
        virtual ~GLES2Object();

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
