#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

#include "GLES2Header.h"
#include <unordered_map>

namespace Supernova {
    class GLES2Object: public ObjectRender{

        struct bufferGlData{
            GLuint buffer = 0;
            GLuint size = 0;
        };
        
        struct attributeGlData{
            GLint handle = -1;
        };

        struct propertyGlData{
            GLint handle = -1;
        };

        struct textureGlData{
            GLint location = -1;
            unsigned int arraySize = 1;
        };
        
    private:
        
        GLuint useTexture;
        GLint maxTextureUnits;

        std::unordered_map<std::string, bufferGlData> vertexBuffersGL;
        std::unordered_map<int, attributeGlData> attributesGL;
        std::unordered_map<int, propertyGlData> propertyGL;
        std::unordered_map<int, textureGlData> texturesGL;
        
    protected:

        int textureIndex;

        void loadBuffer(std::string name, bufferData buff);
        bufferGlData getVertexBufferGL(std::string name);

    public:
        GLES2Object();
        virtual ~GLES2Object();

        virtual void updateBuffer(std::string name, unsigned int size, void* data);

        virtual bool load();
        virtual bool prepareDraw();
        virtual bool draw();
        virtual bool finishDraw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
