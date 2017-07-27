#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"
#include "GLES2Light.h"
#include "GLES2Fog.h"

#include "GLES2Header.h"
#include <unordered_map>

namespace Supernova {
    class GLES2Object: public ObjectRender{
        
        struct attributeGlData{
            GLuint buffer;
            GLuint size;
            GLint handle;
        };
        
    private:
        
        GLenum usageBuffer;
        
        GLES2Light light;
        GLES2Fog fog;
        
        GLuint useTexture;
        GLuint uTextureUnitLocation;
        
        std::unordered_map<int, attributeGlData> attributeBuffers;
        attributeGlData indexBuffer;
        std::unordered_map<int, GLuint> propertyHandle;
        
    protected:
        
        void loadVertexAttribute(int type, attributeData att);
        void loadIndex(attributeData att);
        
        void useProperty(int type, propertyData prop);

    public:
        GLES2Object();
        virtual ~GLES2Object();
        
        virtual void updateVertexAttribute(int type, unsigned long size);
        virtual void updateIndex(unsigned long size);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
