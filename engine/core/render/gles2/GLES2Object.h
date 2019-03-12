#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

#include "GLES2Header.h"
#include <unordered_map>

namespace Supernova {
    class GLES2Object: public ObjectRender{

        struct BufferGlData{
            GLuint buffer = 0;
            GLuint size = 0;
        };
        
        struct AttributeGlData{
            GLint handle = -1;
        };

        struct PropertyGlData{
            GLint handle = -1;
        };

        struct TextureGlData{
            GLint location = -1;
            unsigned int arraySize = 1;
        };
        
    private:
        
        GLuint useTexture;
        GLint maxTextureUnits;

        std::unordered_map<std::string, BufferGlData> vertexBuffersGL;
        std::unordered_map<int, AttributeGlData> attributesGL;
        std::unordered_map<int, PropertyGlData> propertyGL;
        std::unordered_map<int, TextureGlData> texturesGL;
        
    protected:

        int textureIndex;

        void loadBuffer(std::string name, BufferData buff);
        BufferGlData getVertexBufferGL(std::string name);

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
