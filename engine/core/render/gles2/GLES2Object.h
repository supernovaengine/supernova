#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

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
        };
        
        struct propertyGlData{
            GLint handle = -1;
        };
        
    private:
        
        GLenum usageBuffer;
        
        GLuint useTexture;
        GLuint uTextureUnitLocation;

        GLuint uShadowsMapLocation;
        
        std::unordered_map<int, attributeGlData> attributesGL;
        indexGlData indexGL;
        std::unordered_map<int, propertyGlData> propertyGL;
        
    protected:
        
        void loadVertexAttribute(int type, attributeData att);
        void loadIndex(indexData att);

    public:
        GLES2Object();
        virtual ~GLES2Object();
        
        virtual void updateVertexAttribute(int type, unsigned long size, void* data);
        virtual void updateIndex(unsigned long size, void* data);

        virtual bool load();
        virtual bool prepareDraw();
        virtual bool draw();
        virtual bool finishDraw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
