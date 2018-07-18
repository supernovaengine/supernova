#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

#include "GLES2Header.h"
#include <unordered_map>

namespace Supernova {
    class GLES2Object: public ObjectRender{

        struct bufferGlData{
            GLuint buffer = -1;
            GLuint size = 0;
        };
        
        struct attributeGlData{
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
        
        GLuint useTexture;
        GLuint uTextureUnitLocation;

        GLuint uShadowsMap2DLocation;
        GLuint uShadowsMapCubeLocation;

        std::unordered_map<std::string, bufferGlData> vertexBuffersGL;
        std::unordered_map<int, attributeGlData> attributesGL;
        indexGlData indexGL;
        std::unordered_map<int, propertyGlData> propertyGL;
        
    protected:

        void loadVertexBuffer(std::string name, bufferData buff);
        void loadIndex(indexData ibuff);

    public:
        GLES2Object();
        virtual ~GLES2Object();

        virtual void updateVertexBuffer(std::string name, unsigned int size, void* data);
        virtual void updateIndex(unsigned int size, void* data);

        virtual bool load();
        virtual bool prepareDraw();
        virtual bool draw();
        virtual bool finishDraw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
