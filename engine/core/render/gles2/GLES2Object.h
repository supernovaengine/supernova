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
            GLuint buffer = -1;
            GLuint size = 0;
            GLint handle = -1;
        };
        
        struct indexGlData{
            GLuint buffer = -1;
            GLuint size = 0;
            GLint handle = -1;
        };
        
        struct propertyGlData{
            GLint handle = -1;
        };
        
    private:
        
        GLenum usageBuffer;
        
        GLES2Light light;
        GLES2Fog fog;
        
        GLuint useTexture;
        GLuint uTextureUnitLocation;
        
        std::unordered_map<int, attributeGlData> attributesGL;
        attributeGlData indexGL;
        std::unordered_map<int, propertyGlData> propertyGL;
        
    protected:
        
        void loadVertexAttribute(int type, attributeData att);
        void loadIndex(indexData att);
        
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
