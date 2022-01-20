#include "SokolObject.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "Log.h"

using namespace Supernova;


SokolObject::SokolObject(){
    bind = {}; 
    pip.id = SG_INVALID_ID;
    pipeline_desc = {};
    bindSlotIndex = 0;
}

SokolObject::SokolObject(const SokolObject& rhs) {
    bind = rhs.bind; 
    pip = rhs.pip;
    pipeline_desc = rhs.pipeline_desc;
    bindSlotIndex = rhs.bindSlotIndex;
    bufferToBindSlot = rhs.bufferToBindSlot;
}

SokolObject& SokolObject::operator=(const SokolObject& rhs) { 
    bind = rhs.bind; 
    pip = rhs.pip;
    pipeline_desc = rhs.pipeline_desc;
    bindSlotIndex = rhs.bindSlotIndex;
    bufferToBindSlot = rhs.bufferToBindSlot;
    
    return *this;
}

sg_vertex_format SokolObject::getVertexFormat(unsigned int elements, AttributeDataType dataType, bool normalized){
    if (dataType == AttributeDataType::BYTE){
        if (elements == 4)
            return (normalized)?SG_VERTEXFORMAT_BYTE4N:SG_VERTEXFORMAT_BYTE4;
    }
    if (dataType == AttributeDataType::UNSIGNED_BYTE){
        if (elements == 4)
            return (normalized)?SG_VERTEXFORMAT_UBYTE4N:SG_VERTEXFORMAT_UBYTE4;
    }
    if (dataType == AttributeDataType::SHORT){
        if (elements == 2)
            return (normalized)?SG_VERTEXFORMAT_SHORT2N:SG_VERTEXFORMAT_SHORT2;
        if (elements == 4)
            return (normalized)?SG_VERTEXFORMAT_SHORT4N:SG_VERTEXFORMAT_SHORT4;
    }
    if (dataType == AttributeDataType::UNSIGNED_SHORT){
        if (elements == 2)
            return SG_VERTEXFORMAT_USHORT2N;
        if (elements == 4)
            return SG_VERTEXFORMAT_USHORT4N;
    }
    if (dataType == AttributeDataType::FLOAT){
        if (elements == 1)
            return SG_VERTEXFORMAT_FLOAT;
        if (elements == 2)
            return SG_VERTEXFORMAT_FLOAT2;
        if (elements == 3)
            return SG_VERTEXFORMAT_FLOAT3;
        if (elements == 4)
            return SG_VERTEXFORMAT_FLOAT4;
    }
    return SG_VERTEXFORMAT_INVALID;
}

sg_primitive_type SokolObject::getPrimitiveType(PrimitiveType primitiveType){
    if (primitiveType == PrimitiveType::TRIANGLES)
        return SG_PRIMITIVETYPE_TRIANGLES;
    else if (primitiveType == PrimitiveType::TRIANGLE_STRIP)
        return SG_PRIMITIVETYPE_TRIANGLE_STRIP;
    else if (primitiveType == PrimitiveType::LINES)
        return SG_PRIMITIVETYPE_LINES;
    else if (primitiveType == PrimitiveType::POINTS)
        return SG_PRIMITIVETYPE_POINTS;

    return SG_PRIMITIVETYPE_TRIANGLES;
}

void SokolObject::beginLoad(PrimitiveType primitiveType, bool depth, bool renderToTexture){
    bind = {0};
    pip = {0};
    pipeline_desc = {0};

    if (depth){
        pipeline_desc.cull_mode = SG_CULLMODE_FRONT;
        pipeline_desc.sample_count = 1;
        pipeline_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pipeline_desc.depth.write_enabled = true;
        pipeline_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    }else{
        //pipeline_desc.cull_mode = SG_CULLMODE_FRONT;
        pipeline_desc.face_winding = SG_FACEWINDING_CW;

        pipeline_desc.depth.write_enabled = true;
        pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;

        if (renderToTexture){
            pipeline_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
            pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
            pipeline_desc.depth.write_enabled = true;

            pipeline_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
        }

        pipeline_desc.colors[0].write_mask = SG_COLORMASK_RGB;
        pipeline_desc.colors[0].blend.enabled = true;
        pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
        pipeline_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    }

    pipeline_desc.primitive_type = getPrimitiveType(primitiveType);

    bindSlotIndex = 0;
    bufferToBindSlot.clear();
}

void SokolObject::addIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset){
    sg_buffer ibuf = buffer->backend.get();
    bind.index_buffer = ibuf;
    bind.index_buffer_offset = offset;

    if (dataType == AttributeDataType::UNSIGNED_SHORT){
        pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
    }else if (dataType == AttributeDataType::UNSIGNED_INT){
        pipeline_desc.index_type = SG_INDEXTYPE_UINT32;
    }else{
        pipeline_desc.index_type = SG_INDEXTYPE_NONE;
    }
}

void SokolObject::addAttribute(int slotAttribute, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized){
    if (slotAttribute != -1){
        sg_buffer vbuf = buffer->backend.get();
        
        if (bufferToBindSlot.count(vbuf.id) == 0){
            bind.vertex_buffers[bindSlotIndex] = vbuf;
            bufferToBindSlot[vbuf.id] = bindSlotIndex;

            pipeline_desc.layout.buffers[bindSlotIndex].stride = stride;

            bindSlotIndex++;
        }

        size_t indexBuf = bufferToBindSlot[vbuf.id];

        pipeline_desc.layout.attrs[slotAttribute].buffer_index = indexBuf;
        pipeline_desc.layout.attrs[slotAttribute].offset = offset;
        pipeline_desc.layout.attrs[slotAttribute].format = getVertexFormat(elements, dataType, normalized);
    }
}

void SokolObject::addShader(ShaderRender* shader){
    pipeline_desc.shader = shader->backend.get();
}

void SokolObject::addTexture(int slotTexture, ShaderStageType stage, TextureRender* texture){
    if (slotTexture != -1){
        sg_image image = texture->backend.get();
        if (stage == ShaderStageType::VERTEX){
            bind.vs_images[slotTexture] = image;
        }else if (stage == ShaderStageType::FRAGMENT){
            bind.fs_images[slotTexture] = image;
        }

    }
}

void SokolObject::endLoad(){
    pip = sg_make_pipeline(&pipeline_desc);
}

void SokolObject::beginDraw(){
    sg_apply_pipeline(pip);
    sg_apply_bindings(&bind);
}

void SokolObject::applyUniformBlock(int slotUniform, ShaderStageType stage, unsigned int count, void* data){
    if (slotUniform != -1){
        sg_shader_stage sg_stage;
        if (stage == ShaderStageType::VERTEX){
            sg_stage = SG_SHADERSTAGE_VS;
        }else if (stage == ShaderStageType::FRAGMENT){
            sg_stage = SG_SHADERSTAGE_FS;
        }
        sg_apply_uniforms(sg_stage, slotUniform, {data, count});
    }
}

void SokolObject::draw(int vertexCount){
    sg_draw(0, vertexCount, 1);
}

void SokolObject::destroy(){
    if (pip.id != SG_INVALID_ID && sg_isvalid()){
        sg_destroy_pipeline(pip);
    }

    pip.id = SG_INVALID_ID;
}