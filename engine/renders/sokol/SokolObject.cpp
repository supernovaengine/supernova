#include "SokolObject.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "shaders/shaders.h"
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

size_t SokolObject::getAttributesIndex(AttributeType type, ShaderType shaderType){
    if (type == AttributeType::POSITIONS){
        if (shaderType == ShaderType::MESH)
            return ATTR_mesh_vs_a_position;
        if (shaderType == ShaderType::MESH_PBR_UNLIT)
            return ATTR_meshPBR_unlit_vs_a_position;
        if (shaderType == ShaderType::MESH_PBR)
            return ATTR_meshPBR_vs_a_position;
        if (shaderType == ShaderType::MESH_PBR_NONMAP_NOTAN)
            return ATTR_meshPBR_noNmap_noTan_vs_a_position;
        if (shaderType == ShaderType::SKYBOX)
            return ATTR_skybox_vs_a_position;
    }else if (type == AttributeType::TEXTURECOORDS){
        if (shaderType == ShaderType::MESH_PBR_UNLIT)
            return ATTR_meshPBR_unlit_vs_a_texcoord1;
        if (shaderType == ShaderType::MESH_PBR)
            return ATTR_meshPBR_vs_a_texcoord1;
        if (shaderType == ShaderType::MESH_PBR_NONMAP_NOTAN)
            return ATTR_meshPBR_noNmap_noTan_vs_a_texcoord1;
    }else if (type == AttributeType::NORMALS){
        if (shaderType == ShaderType::MESH_PBR)
            return ATTR_meshPBR_vs_a_normal;
        if (shaderType == ShaderType::MESH_PBR_NONMAP_NOTAN)
            return ATTR_meshPBR_noNmap_noTan_vs_a_normal;
    }else if (type == AttributeType::COLORS){
        if (shaderType == ShaderType::MESH_PBR)
            return ATTR_meshPBR_vs_a_color;
        if (shaderType == ShaderType::MESH_PBR_NONMAP_NOTAN)
            return ATTR_meshPBR_noNmap_noTan_vs_a_color;
    }else if (type == AttributeType::TANGENTS){
        if (shaderType == ShaderType::MESH_PBR)
            return ATTR_meshPBR_vs_a_tangent;
    }

    return -1;
}

size_t SokolObject::getTextureSampler(TextureSamplerType samplerType){
    if (samplerType == TextureSamplerType::BASECOLOR){
        return SLOT_u_baseColorTexture;
    }else if (samplerType == TextureSamplerType::METALLICROUGHNESS){
        return SLOT_u_metallicRoughnessTexture;
    }else if (samplerType == TextureSamplerType::EMISSIVE){
        return SLOT_u_emissiveTexture;
    }else if (samplerType == TextureSamplerType::NORMAL){
        return SLOT_u_normalTexture;
    }else if (samplerType == TextureSamplerType::OCCULSION){
        return SLOT_u_occlusionTexture;
    }else if (samplerType == TextureSamplerType::SKYCUBE){
        return SLOT_u_skyTexture;
    }

    return -1;
}

sg_vertex_format SokolObject::getVertexFormat(unsigned int elements, AttributeDataType dataType, bool normalized){
    if (dataType == AttributeDataType::BYTE){
        if (elements == 4)
            if (normalized)
                return SG_VERTEXFORMAT_BYTE4N;
            else
                return SG_VERTEXFORMAT_BYTE4;
    }
    if (dataType == AttributeDataType::UNSIGNED_BYTE){
        if (elements == 4)
            if (normalized)
                return SG_VERTEXFORMAT_UBYTE4N;
            else
                return SG_VERTEXFORMAT_UBYTE4;
    }
    if (dataType == AttributeDataType::SHORT){
        if (elements == 2)
            if (normalized)
                return SG_VERTEXFORMAT_SHORT2N;
            else
                return SG_VERTEXFORMAT_SHORT2;
        if (elements == 4)
            if (normalized)
                return SG_VERTEXFORMAT_SHORT4N;
            else
                return SG_VERTEXFORMAT_SHORT4;
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

UniformStageSlot SokolObject::getUniformStageSlot(UniformType type){

    //Vertex uniforms
    if (type == UniformType::TRANSFORM){
        return {SG_SHADERSTAGE_VS, SLOT_transform};
    }else if (type == UniformType::VIEWPROJECTIONSKY){
        return {SG_SHADERSTAGE_VS, SLOT_viewProjectionSky};
    //}else if (type == UniformType::PBR_VS_PARAMS){
    //    return {SG_SHADERSTAGE_VS, SLOT_u_pbr_vs_params};
    }

    //Fragment uniforms
    if (type == UniformType::MATERIAL){
        return {SG_SHADERSTAGE_FS, SLOT_u_material};
    }else if (type == UniformType::PBR_FS_PARAMS){
        return {SG_SHADERSTAGE_FS, SLOT_u_fs_pbrParams};
    }else if (type == UniformType::DIR_LIGHTS){
        return {SG_SHADERSTAGE_FS, SLOT_u_dirLight};
    //}else if (type == UniformType::LIGHTING){
    //    return {SG_SHADERSTAGE_FS, SLOT_u_lighting};
    }

    return {SG_SHADERSTAGE_VS, -1};
}

void SokolObject::beginLoad(PrimitiveType primitiveType){
    bind = {0};
    pip = {0};
    pipeline_desc = {0};

    //pipeline_desc.cull_mode = SG_CULLMODE_FRONT;
    pipeline_desc.face_winding = SG_FACEWINDING_CW;

    pipeline_desc.depth.write_enabled = true;
    pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;

    pipeline_desc.colors[0].write_mask = SG_COLORMASK_RGB;
    pipeline_desc.colors[0].blend.enabled = true;
    pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pipeline_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    pipeline_desc.primitive_type = getPrimitiveType(primitiveType);

    bindSlotIndex = 0;
    bufferToBindSlot.clear();
}

void SokolObject::loadIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset){
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
 
void SokolObject::loadAttribute(AttributeType type, ShaderType shaderType, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized){
    size_t indexAttr = getAttributesIndex(type, shaderType);

    if (indexAttr != -1){
        sg_buffer vbuf = buffer->backend.get();
        
        if (bufferToBindSlot.count(vbuf.id) == 0){
            bind.vertex_buffers[bindSlotIndex] = vbuf;
            bufferToBindSlot[vbuf.id] = bindSlotIndex;

            pipeline_desc.layout.buffers[bindSlotIndex].stride = stride;

            bindSlotIndex++;
        }

        size_t indexBuf = bufferToBindSlot[vbuf.id];

        pipeline_desc.layout.attrs[indexAttr].buffer_index = indexBuf;
        pipeline_desc.layout.attrs[indexAttr].offset = offset;
        pipeline_desc.layout.attrs[indexAttr].format = getVertexFormat(elements, dataType, normalized);
    }
}

void SokolObject::loadShader(ShaderRender* shader){
    pipeline_desc.shader = shader->backend.get();
}

void SokolObject::loadTexture(TextureRender* texture, TextureSamplerType samplerType){
    sg_image image = texture->backend.get();
    size_t texSampler = getTextureSampler(samplerType);
    if (texSampler != -1)
        bind.fs_images[texSampler] = image;
}

void SokolObject::endLoad(){
    pip = sg_make_pipeline(&pipeline_desc);
}

void SokolObject::beginDraw(){
    sg_apply_pipeline(pip);
    sg_apply_bindings(&bind);
}

void SokolObject::applyUniform(UniformType type, UniformDataType datatype, unsigned int count, void* data){
    if (datatype == UniformDataType::FLOAT){
        UniformStageSlot uniform = getUniformStageSlot(type);
        sg_apply_uniforms(uniform.stage, uniform.slot, {data, sizeof(float) * count});
    }else{
        Log::Error("Sokol backend only supports float uniforms");
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