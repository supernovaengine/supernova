// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "SokolObject.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "Log.h"
#include "SokolCmdQueue.h"
#include "SokolShader.h"
#include "Engine.h"

using namespace doriax;

SokolObject::SokolObject(){
    pip.id = SG_INVALID_ID;
    depth_pip.id = SG_INVALID_ID;
    shadow_depth_pip.id = SG_INVALID_ID;
    rtt_pip.id = SG_INVALID_ID;
    rtt_invert_pip.id = SG_INVALID_ID;
    gbuffer_pip.id = SG_INVALID_ID;
    bind = {};
    pipeline_desc = {};
    bindSlotIndex = 0;
}

SokolObject::SokolObject(const SokolObject& rhs) {
    bind = rhs.bind;
    pip = rhs.pip;
    depth_pip = rhs.depth_pip;
    shadow_depth_pip = rhs.shadow_depth_pip;
    rtt_pip = rhs.rtt_pip;
    rtt_invert_pip = rhs.rtt_invert_pip;
    gbuffer_pip = rhs.gbuffer_pip;
    pipeline_desc = rhs.pipeline_desc;
    bindSlotIndex = rhs.bindSlotIndex;
    bufferToBindSlot = rhs.bufferToBindSlot;
}

SokolObject& SokolObject::operator=(const SokolObject& rhs) {
    bind = rhs.bind;
    pip = rhs.pip;
    depth_pip = rhs.depth_pip;
    shadow_depth_pip = rhs.shadow_depth_pip;
    rtt_pip = rhs.rtt_pip;
    rtt_invert_pip = rhs.rtt_invert_pip;
    gbuffer_pip = rhs.gbuffer_pip;
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

    return _SG_PRIMITIVETYPE_DEFAULT;
}

sg_cull_mode SokolObject::getCullMode(CullingMode cullingMode){
    if (cullingMode == CullingMode::BACK){
        return SG_CULLMODE_BACK;
    }else if (cullingMode == CullingMode::FRONT){
        return SG_CULLMODE_FRONT;
    }

    return _SG_CULLMODE_DEFAULT;
}

sg_face_winding SokolObject::getFaceWinding(WindingOrder windingOrder){
    if (windingOrder == WindingOrder::CCW){
        return SG_FACEWINDING_CCW;
    }else if (windingOrder == WindingOrder::CW){
        return SG_FACEWINDING_CW;
    }

    return _SG_FACEWINDING_DEFAULT;
}

void SokolObject::beginLoad(PrimitiveType primitiveType){
    bind = {0};
    pip = {0};
    depth_pip = {0};
    shadow_depth_pip = {0};
    rtt_pip = {0};
    rtt_invert_pip = {0};
    gbuffer_pip = {0};
    pipeline_desc = {0};

    pipeline_desc.primitive_type = getPrimitiveType(primitiveType);

    bindSlotIndex = 0;
    bufferToBindSlot.clear();
}

void SokolObject::setShader(ShaderRender* shader){
    pipeline_desc.shader = shader->backend.get();
}

void SokolObject::setIndex(BufferRender* buffer, AttributeDataType dataType, size_t offset){
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

void SokolObject::addAttribute(int slot, BufferRender* buffer, unsigned int elements, AttributeDataType dataType, unsigned int stride, size_t offset, bool normalized, bool perInstance){
    if (slot != -1){
        sg_buffer vbuf = buffer->backend.get();

        // D3D11 cannot have offset (AlignedByteOffset) bigger than 2048
        // https://github.com/floooh/sokol/issues/818
        // Metal also cannot use large offsets
        size_t bufferOffset = 0;
        size_t attrOffset = 0;

        if (Engine::isOpenGL()){
            bufferOffset = 0;
            attrOffset = offset;
        }else{
            bufferOffset = offset;
            attrOffset = 0;
        }
        
        if (bufferToBindSlot.count({vbuf.id, bufferOffset}) == 0){
            bind.vertex_buffers[bindSlotIndex] = vbuf;
            bind.vertex_buffer_offsets[bindSlotIndex] = bufferOffset;
            bufferToBindSlot[{vbuf.id, bufferOffset}] = bindSlotIndex;

            pipeline_desc.layout.buffers[bindSlotIndex].stride = stride;

            if (perInstance){
                pipeline_desc.layout.buffers[bindSlotIndex].step_func = SG_VERTEXSTEP_PER_INSTANCE;
            }

            bindSlotIndex++;
        }

        size_t indexBuf = bufferToBindSlot[{vbuf.id, bufferOffset}];

        pipeline_desc.layout.attrs[slot].buffer_index = indexBuf;
        pipeline_desc.layout.attrs[slot].offset = attrOffset;
        pipeline_desc.layout.attrs[slot].format = getVertexFormat(elements, dataType, normalized);
    }
}

void SokolObject::replaceVertexBuffer(uint32_t fromBufferId, sg_buffer toBuffer){
    // A single interleaved buffer can occupy several bind slots (one per attribute
    // offset on Metal/D3D), so swap every slot that referenced the original buffer.
    for (auto const& kv : bufferToBindSlot){
        if (kv.first.id == fromBufferId){
            bind.vertex_buffers[kv.second] = toBuffer;
        }
    }
}

void SokolObject::addStorageBuffer(int slot, ShaderStageType stage, BufferRender* buffer){
    if (slot != -1){
        sg_view sbufview = buffer->backend.getView();

        if (stage == ShaderStageType::VERTEX){
            bind.views[SOKOL_STORAGEBUFFER_VIEW_SLOT_OFFSET + slot] = sbufview;
        }else if (stage == ShaderStageType::FRAGMENT){
            bind.views[SOKOL_STORAGEBUFFER_VIEW_SLOT_OFFSET + slot] = sbufview;
        }
    }
}

void SokolObject::addTexture(std::pair<int, int> slot, ShaderStageType stage, TextureRender* texture){
    if (slot.first != -1){
        sg_view texview = texture->backend.getView();
        sg_sampler sampler = texture->backend.getSampler();
        if (stage == ShaderStageType::VERTEX){
            bind.views[slot.first] = texview;
            bind.samplers[slot.second] = sampler;
        }else if (stage == ShaderStageType::FRAGMENT){
            bind.views[slot.first] = texview;
            bind.samplers[slot.second] = sampler;
        }
    }
}

bool SokolObject::endLoad(uint8_t pipelines, bool enableFaceCulling, bool enableDepthWrite, CullingMode cullingMode, WindingOrder windingOrder){

    if (pipelines & (int)PipelineType::PIP_DEPTH) {
        sg_pipeline_desc pip_depth_desc = pipeline_desc;

        if (enableFaceCulling){
            pip_depth_desc.cull_mode = getCullMode(cullingMode);
            pip_depth_desc.face_winding = getFaceWinding(windingOrder);
        }

        pip_depth_desc.sample_count = 1;
        pip_depth_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_depth_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_depth_desc.depth.write_enabled = true;
        pip_depth_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;

        if (Engine::isAsyncThread()){
            depth_pip = SokolCmdQueue::add_command_make_pipeline(pip_depth_desc);
        }else{
            depth_pip = sg_make_pipeline(pip_depth_desc);
        }

        if (depth_pip.id == SG_INVALID_ID){
            return false;
        }
    }

    if (pipelines & (int)PipelineType::PIP_SHADOW_DEPTH) {
        // Depth-only pipeline (no color target); keeps the fragment stage for
        // alpha-mask discard. SG_PIXELFORMAT_NONE marks it depth-only in Sokol.
        sg_pipeline_desc pip_shadow_depth_desc = pipeline_desc;

        if (enableFaceCulling){
            pip_shadow_depth_desc.cull_mode = getCullMode(cullingMode);
            pip_shadow_depth_desc.face_winding = getFaceWinding(windingOrder);
        }

        pip_shadow_depth_desc.sample_count = 1;
        pip_shadow_depth_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_shadow_depth_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_shadow_depth_desc.depth.write_enabled = true;
        pip_shadow_depth_desc.color_count = 0;
        pip_shadow_depth_desc.colors[0].pixel_format = SG_PIXELFORMAT_NONE;

        if (Engine::isAsyncThread()){
            shadow_depth_pip = SokolCmdQueue::add_command_make_pipeline(pip_shadow_depth_desc);
        }else{
            shadow_depth_pip = sg_make_pipeline(pip_shadow_depth_desc);
        }

        if (shadow_depth_pip.id == SG_INVALID_ID){
            return false;
        }
    }

    if (pipelines & (int)PipelineType::PIP_GBUFFER) {
        // Mesh geometry pass with three color attachments (MRT):
        //   color[0] = packed depth (same as PIP_DEPTH, so SSAO/SSR depth is unchanged)
        //   color[1] = view-space normal (octahedral .rg) + roughness (.b) + metallic (.a)
        //   color[2] = linear base color (.rgb) + hasIBL flag (.a)
        sg_pipeline_desc pip_gbuffer_desc = pipeline_desc;

        if (enableFaceCulling){
            pip_gbuffer_desc.cull_mode = getCullMode(cullingMode);
            pip_gbuffer_desc.face_winding = getFaceWinding(windingOrder);
        }

        pip_gbuffer_desc.sample_count = 1;
        pip_gbuffer_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_gbuffer_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_gbuffer_desc.depth.write_enabled = true;
        pip_gbuffer_desc.color_count = 3;
        pip_gbuffer_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
        pip_gbuffer_desc.colors[1].pixel_format = SG_PIXELFORMAT_RGBA8;
        pip_gbuffer_desc.colors[2].pixel_format = SG_PIXELFORMAT_RGBA8;

        if (Engine::isAsyncThread()){
            gbuffer_pip = SokolCmdQueue::add_command_make_pipeline(pip_gbuffer_desc);
        }else{
            gbuffer_pip = sg_make_pipeline(pip_gbuffer_desc);
        }

        if (gbuffer_pip.id == SG_INVALID_ID){
            return false;
        }
    }

    if (pipelines & (int)PipelineType::PIP_DEFAULT) {
        sg_pipeline_desc pip_default_desc = pipeline_desc;

        if (enableFaceCulling){
            pip_default_desc.cull_mode = getCullMode(cullingMode);
            pip_default_desc.face_winding = getFaceWinding(windingOrder);
        }

        pip_default_desc.depth.write_enabled = enableDepthWrite;
        pip_default_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;

        pip_default_desc.colors[0].write_mask = SG_COLORMASK_RGB;
        pip_default_desc.colors[0].blend.enabled = true;
        pip_default_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
        pip_default_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

        if (Engine::isAsyncThread()){
            pip = SokolCmdQueue::add_command_make_pipeline(pip_default_desc);
        }else{
            pip = sg_make_pipeline(pip_default_desc);
        }

        if (pip.id == SG_INVALID_ID){
            return false;
        }
    }

    // PIP_RTT (offscreen) and PIP_RTT_INVERT (planar reflection) share an identical
    // pipeline except for triangle winding, so build the common desc once.
    if (pipelines & ((int)PipelineType::PIP_RTT | (int)PipelineType::PIP_RTT_INVERT)){
        sg_pipeline_desc pip_rtt_desc = pipeline_desc;

        pip_rtt_desc.sample_count = 1;
        pip_rtt_desc.depth.write_enabled = enableDepthWrite;
        pip_rtt_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_rtt_desc.colors[0].write_mask = SG_COLORMASK_RGBA;
        pip_rtt_desc.colors[0].blend.enabled = true;
        pip_rtt_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
        pip_rtt_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        pip_rtt_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
        pip_rtt_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        pip_rtt_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_rtt_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;

        // offscreen passes render with a Y-flipped projection on GL (top-left
        // origin), which reverses the triangle winding
        WindingOrder rttWinding = windingOrder;
        if (Engine::isOpenGL()){
            rttWinding = (windingOrder == WindingOrder::CCW) ? WindingOrder::CW : WindingOrder::CCW;
        }
        if (enableFaceCulling){
            pip_rtt_desc.cull_mode = getCullMode(cullingMode);
        }

        if (pipelines & (int)PipelineType::PIP_RTT){
            if (enableFaceCulling){
                pip_rtt_desc.face_winding = getFaceWinding(rttWinding);
            }
            if (Engine::isAsyncThread()){
                rtt_pip = SokolCmdQueue::add_command_make_pipeline(pip_rtt_desc);
            }else{
                rtt_pip = sg_make_pipeline(pip_rtt_desc);
            }
            if (rtt_pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_RTT_INVERT){
            // planar reflection mirrors handedness, reversing winding once more
            if (enableFaceCulling){
                WindingOrder invWinding = (rttWinding == WindingOrder::CCW) ? WindingOrder::CW : WindingOrder::CCW;
                pip_rtt_desc.face_winding = getFaceWinding(invWinding);
            }
            if (Engine::isAsyncThread()){
                rtt_invert_pip = SokolCmdQueue::add_command_make_pipeline(pip_rtt_desc);
            }else{
                rtt_invert_pip = sg_make_pipeline(pip_rtt_desc);
            }
            if (rtt_invert_pip.id == SG_INVALID_ID){
                return false;
            }
        }
    }

    return true;
}

bool SokolObject::beginDraw(PipelineType pipType){
    sg_pipeline selectedPipeline = pip;
    if (pipType == PipelineType::PIP_DEPTH){
        selectedPipeline = depth_pip;
    }else if (pipType == PipelineType::PIP_SHADOW_DEPTH){
        selectedPipeline = shadow_depth_pip;
    }else if (pipType == PipelineType::PIP_GBUFFER){
        selectedPipeline = gbuffer_pip;
    }else if (pipType == PipelineType::PIP_RTT){
        selectedPipeline = rtt_pip;
    }else if (pipType == PipelineType::PIP_RTT_INVERT){
        selectedPipeline = rtt_invert_pip;
    }

    // Deferred resource creation can leave an allocated handle in FAILED state.
    // Never apply it: doing so would turn one creation error into a cascade of
    // invalid pipeline, uniform, binding, and draw validation failures.
    if (selectedPipeline.id == SG_INVALID_ID ||
        sg_query_pipeline_state(selectedPipeline) != SG_RESOURCESTATE_VALID){
        return false;
    }

    sg_apply_pipeline(selectedPipeline);
    return true;
}

void SokolObject::applyUniformBlock(int slot, unsigned int count, void* data){
    if (slot != -1){
        sg_apply_uniforms(slot, {data, count});
    }
}

void SokolObject::draw(unsigned int baseElement, unsigned int vertexCount, unsigned int instanceCount){
    //SokolCmdQueue::add_command_apply_bindings(bind);
    sg_apply_bindings(bind);
    //SokolCmdQueue::add_command_draw(0, vertexCount, 1);
    sg_draw(baseElement, vertexCount, instanceCount);
}

void SokolObject::destroy(){
    if (sg_isvalid()){
        if (pip.id != SG_INVALID_ID){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_pipeline(pip);
            }else{
                sg_destroy_pipeline(pip);
            }
        }
        if (depth_pip.id != SG_INVALID_ID){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_pipeline(depth_pip);
            }else{
                sg_destroy_pipeline(depth_pip);
            }
        }
        if (shadow_depth_pip.id != SG_INVALID_ID){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_pipeline(shadow_depth_pip);
            }else{
                sg_destroy_pipeline(shadow_depth_pip);
            }
        }
        if (rtt_pip.id != SG_INVALID_ID){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_pipeline(rtt_pip);
            }else{
                sg_destroy_pipeline(rtt_pip);
            }
        }
        if (rtt_invert_pip.id != SG_INVALID_ID){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_pipeline(rtt_invert_pip);
            }else{
                sg_destroy_pipeline(rtt_invert_pip);
            }
        }
        if (gbuffer_pip.id != SG_INVALID_ID){
            if (Engine::isAsyncThread()){
                SokolCmdQueue::add_command_destroy_pipeline(gbuffer_pip);
            }else{
                sg_destroy_pipeline(gbuffer_pip);
            }
        }
    }

    pip.id = SG_INVALID_ID;
    depth_pip.id = SG_INVALID_ID;
    shadow_depth_pip.id = SG_INVALID_ID;
    rtt_pip.id = SG_INVALID_ID;
    rtt_invert_pip.id = SG_INVALID_ID;
    gbuffer_pip.id = SG_INVALID_ID;
    bind = {};
    pipeline_desc = {};
    bindSlotIndex = 0;
}
