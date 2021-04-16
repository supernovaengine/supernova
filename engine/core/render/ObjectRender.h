#ifndef ObjectRender_h
#define ObjectRender_h

#include "texture/Texture.h"
#include "Render.h"

#include "sokol/SokolObject.h"

namespace Supernova {

    class ObjectRender{

    public:

        //***Backend***
        SokolObject backend;
        //***

        ObjectRender();
        ObjectRender(const ObjectRender& rhs);
        ObjectRender& operator=(const ObjectRender& rhs);
        virtual ~ObjectRender();

        void beginLoad(PrimitiveType primitiveType);
        void loadIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset);
        void loadAttribute(AttributeType type, ShaderType shaderType, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized);
        void loadShader(ShaderRender* shader);
        void loadTexture(TextureRender* texture, TextureSamplerType samplerType);
        void endLoad();

        void beginDraw();
        void applyUniform(UniformType type, UniformDataType datatype, unsigned int size, void* data);
        void draw(int vertexCount);

        void destroy();
    };
}


#endif /* ObjectRender_h */
