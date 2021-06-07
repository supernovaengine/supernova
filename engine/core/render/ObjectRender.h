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

        void beginLoad(PrimitiveType primitiveType, bool depth);
        void loadIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset);
        void loadAttribute(int slotAttribute, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized);
        void loadShader(ShaderRender* shader);
        void loadTexture(int slotTexture, ShaderStageType stage, TextureRender* texture);
        void endLoad();

        void beginDraw();
        void applyUniform(int slotUniform, ShaderStageType stage, UniformDataType datatype, unsigned int count, void* data);
        void draw(int vertexCount);

        void destroy();
    };
}


#endif /* ObjectRender_h */
