#ifndef sokolobject_h
#define sokolobject_h

#include "render/Render.h"
#include "buffer/Attribute.h"
#include "render/BufferRender.h"
#include "render/ShaderRender.h"
#include "render/TextureRender.h"

#include "sokol_gfx.h"

#include <unordered_map>


namespace Supernova{

    struct UniformStageSlot{
        sg_shader_stage stage;
        int slot;
    };

    class SokolObject{

    private:

        sg_bindings bind;
        sg_pipeline pip;
        sg_pipeline_desc pipeline_desc;

        size_t bindSlotIndex;

        std::unordered_map< uint32_t, size_t > bufferToBindSlot;


        size_t getAttributesIndex(AttributeType type, ShaderType shaderType);
        size_t getTextureSampler(TextureShaderType type);
        sg_vertex_format getVertexFormat(unsigned int elements, AttributeDataType dataType, bool normalized);
        sg_primitive_type getPrimitiveType(PrimitiveType primitiveType);

        UniformStageSlot getUniformStageSlot(UniformType type);

    public:

        SokolObject();
        SokolObject(const SokolObject& rhs);
        SokolObject& operator=(const SokolObject& rhs);

        void beginLoad(PrimitiveType primitiveType, bool depth);
        void addIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset);
        void addAttribute(int slotAttribute, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized);
        void addShader(ShaderRender* shader);
        void addTexture(int slotTexture, ShaderStageType stage, TextureRender* texture);
        void endLoad();

        void beginDraw();
        void applyUniform(int slotUniform, ShaderStageType stage, UniformDataType datatype, unsigned int count, void* data);
        void draw(int vertexCount);

        void destroy();

    };
}
#endif //sokolobject_h