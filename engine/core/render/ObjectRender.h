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
        void addIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset);
        void addAttribute(int slotAttribute, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized);
        void addShader(ShaderRender* shader);
        void addTexture(int slotTexture, ShaderStageType stage, TextureRender* texture);
        void endLoad(uint8_t pipelines);

        void beginDraw(PipelineType pipType);
        void applyUniformBlock(int slotUniform, ShaderStageType stage, unsigned int count, void* data);
        void draw(int vertexCount);

        void destroy();
    };
}


#endif /* ObjectRender_h */
