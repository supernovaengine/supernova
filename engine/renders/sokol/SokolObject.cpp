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

namespace {
    sg_pipeline makePipeline(const sg_pipeline_desc& desc){
        if (Engine::isAsyncThread())
            return SokolCmdQueue::add_command_make_pipeline(desc);
        return sg_make_pipeline(desc);
    }

    void destroyPipeline(sg_pipeline pipeline){
        if (pipeline.id == SG_INVALID_ID)
            return;
        if (Engine::isAsyncThread()){
            SokolCmdQueue::add_command_destroy_pipeline(pipeline);
        }else{
            sg_destroy_pipeline(pipeline);
        }
    }
}

SokolObject::SokolObject(){
    pip.id = SG_INVALID_ID;
    depth_pip.id = SG_INVALID_ID;
    shadow_depth_pip.id = SG_INVALID_ID;
    rtt_pip.id = SG_INVALID_ID;
    rtt_invert_pip.id = SG_INVALID_ID;
    gbuffer_pip.id = SG_INVALID_ID;
    nodepth_pip.id = SG_INVALID_ID;
    rtt_nodepth_pip.id = SG_INVALID_ID;
    rtt_noblend_pip.id = SG_INVALID_ID;
    rtt_coloronly_pip.id = SG_INVALID_ID;
    zprepass_pip.id = SG_INVALID_ID;
    zprepass_rtt_pip.id = SG_INVALID_ID;
    bind = {};
    loadIndexBuffer.id = SG_INVALID_ID;
    loadIndexOffset = 0;
    pipeline_desc = {};
    bindSlotIndex = 0;
}

SokolObject::SokolObject(const SokolObject& rhs) {
    bind = rhs.bind;
    loadIndexBuffer = rhs.loadIndexBuffer;
    loadIndexOffset = rhs.loadIndexOffset;
    pip = rhs.pip;
    depth_pip = rhs.depth_pip;
    shadow_depth_pip = rhs.shadow_depth_pip;
    rtt_pip = rhs.rtt_pip;
    rtt_invert_pip = rhs.rtt_invert_pip;
    gbuffer_pip = rhs.gbuffer_pip;
    nodepth_pip = rhs.nodepth_pip;
    rtt_nodepth_pip = rhs.rtt_nodepth_pip;
    rtt_noblend_pip = rhs.rtt_noblend_pip;
    rtt_coloronly_pip = rhs.rtt_coloronly_pip;
    zprepass_pip = rhs.zprepass_pip;
    zprepass_rtt_pip = rhs.zprepass_rtt_pip;
    pipeline_desc = rhs.pipeline_desc;
    bindSlotIndex = rhs.bindSlotIndex;
    bufferToBindSlot = rhs.bufferToBindSlot;
}

SokolObject& SokolObject::operator=(const SokolObject& rhs) {
    bind = rhs.bind;
    loadIndexBuffer = rhs.loadIndexBuffer;
    loadIndexOffset = rhs.loadIndexOffset;
    pip = rhs.pip;
    depth_pip = rhs.depth_pip;
    shadow_depth_pip = rhs.shadow_depth_pip;
    rtt_pip = rhs.rtt_pip;
    rtt_invert_pip = rhs.rtt_invert_pip;
    gbuffer_pip = rhs.gbuffer_pip;
    nodepth_pip = rhs.nodepth_pip;
    rtt_nodepth_pip = rhs.rtt_nodepth_pip;
    rtt_noblend_pip = rhs.rtt_noblend_pip;
    rtt_coloronly_pip = rhs.rtt_coloronly_pip;
    zprepass_pip = rhs.zprepass_pip;
    zprepass_rtt_pip = rhs.zprepass_rtt_pip;
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
    loadIndexBuffer = ibuf;
    loadIndexOffset = offset;

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

void SokolObject::replaceVertexBuffer(uint32_t fromBufferId, sg_buffer toBuffer, size_t byteOffset){
    // A single interleaved buffer can occupy several bind slots (one per attribute
    // offset on Metal/D3D), so swap every slot that referenced the original buffer.
    for (auto const& kv : bufferToBindSlot){
        if (kv.first.id == fromBufferId){
            bind.vertex_buffers[kv.second] = toBuffer;
            bind.vertex_buffer_offsets[kv.second] = (int)(kv.first.offset + byteOffset);
        }
    }
}

void SokolObject::setIndexBuffer(sg_buffer buffer){
    bind.index_buffer = buffer;
    bind.index_buffer_offset = 0;
}

void SokolObject::resetIndexBuffer(){
    bind.index_buffer = loadIndexBuffer;
    bind.index_buffer_offset = (int)loadIndexOffset;
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

bool SokolObject::endLoad(uint16_t pipelines, bool enableFaceCulling, bool enableDepthWrite, CullingMode cullingMode, WindingOrder windingOrder, bool prepassFaceCulling){

    if (pipelines & (int)PipelineType::PIP_DEPTH) {
        sg_pipeline_desc pip_depth_desc = pipeline_desc;

        pip_depth_desc.face_winding = getFaceWinding(windingOrder);
        if (enableFaceCulling){
            pip_depth_desc.cull_mode = getCullMode(cullingMode);
        }

        pip_depth_desc.sample_count = 1;
        pip_depth_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_depth_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_depth_desc.depth.write_enabled = true;
        pip_depth_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;

        depth_pip = makePipeline(pip_depth_desc);
        if (depth_pip.id == SG_INVALID_ID){
            return false;
        }
    }

    // Depth prepass: the depth shader writes only depth into the color pass target, so
    // formats and winding follow that target (PIP_DEFAULT or PIP_RTT) rather than PIP_DEPTH.
    if (pipelines & ((int)PipelineType::PIP_ZPREPASS | (int)PipelineType::PIP_ZPREPASS_RTT)) {
        sg_pipeline_desc pip_zprepass_desc = pipeline_desc;

        pip_zprepass_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_zprepass_desc.depth.write_enabled = true;
        pip_zprepass_desc.colors[0].write_mask = SG_COLORMASK_NONE;

        if (pipelines & (int)PipelineType::PIP_ZPREPASS){
            pip_zprepass_desc.face_winding = getFaceWinding(windingOrder);
            if (prepassFaceCulling){
                pip_zprepass_desc.cull_mode = getCullMode(cullingMode);
            }
            zprepass_pip = makePipeline(pip_zprepass_desc);
            if (zprepass_pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_ZPREPASS_RTT){
            pip_zprepass_desc.sample_count = 1;
            pip_zprepass_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
            pip_zprepass_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
            WindingOrder rttWinding = windingOrder;
            if (Engine::isOpenGL()){
                rttWinding = (windingOrder == WindingOrder::CCW) ? WindingOrder::CW : WindingOrder::CCW;
            }
            pip_zprepass_desc.face_winding = getFaceWinding(rttWinding);
            if (prepassFaceCulling){
                pip_zprepass_desc.cull_mode = getCullMode(cullingMode);
            }
            zprepass_rtt_pip = makePipeline(pip_zprepass_desc);
            if (zprepass_rtt_pip.id == SG_INVALID_ID){
                return false;
            }
        }
    }

    if (pipelines & (int)PipelineType::PIP_SHADOW_DEPTH) {
        // Depth-only pipeline (no color target); keeps the fragment stage for
        // alpha-mask discard. SG_PIXELFORMAT_NONE marks it depth-only in Sokol.
        sg_pipeline_desc pip_shadow_depth_desc = pipeline_desc;

        pip_shadow_depth_desc.face_winding = getFaceWinding(windingOrder);
        if (enableFaceCulling){
            pip_shadow_depth_desc.cull_mode = getCullMode(cullingMode);
        }

        pip_shadow_depth_desc.sample_count = 1;
        pip_shadow_depth_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_shadow_depth_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_shadow_depth_desc.depth.write_enabled = true;
        pip_shadow_depth_desc.color_count = 0;
        pip_shadow_depth_desc.colors[0].pixel_format = SG_PIXELFORMAT_NONE;

        shadow_depth_pip = makePipeline(pip_shadow_depth_desc);
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

        pip_gbuffer_desc.face_winding = getFaceWinding(windingOrder);
        if (enableFaceCulling){
            pip_gbuffer_desc.cull_mode = getCullMode(cullingMode);
        }

        pip_gbuffer_desc.sample_count = 1;
        pip_gbuffer_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
        pip_gbuffer_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pip_gbuffer_desc.depth.write_enabled = true;
        pip_gbuffer_desc.color_count = 3;
        pip_gbuffer_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
        pip_gbuffer_desc.colors[1].pixel_format = SG_PIXELFORMAT_RGBA8;
        pip_gbuffer_desc.colors[2].pixel_format = SG_PIXELFORMAT_RGBA8;

        gbuffer_pip = makePipeline(pip_gbuffer_desc);
        if (gbuffer_pip.id == SG_INVALID_ID){
            return false;
        }
    }

    if (pipelines & ((int)PipelineType::PIP_DEFAULT | (int)PipelineType::PIP_DEFAULT_NODEPTH)) {
        sg_pipeline_desc pip_default_desc = pipeline_desc;

        // also without culling: double-sided materials read gl_FrontFacing
        pip_default_desc.face_winding = getFaceWinding(windingOrder);
        if (enableFaceCulling){
            pip_default_desc.cull_mode = getCullMode(cullingMode);
        }

        pip_default_desc.colors[0].write_mask = SG_COLORMASK_RGB;
        pip_default_desc.colors[0].blend.enabled = true;
        pip_default_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
        pip_default_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

        if (pipelines & (int)PipelineType::PIP_DEFAULT){
            pip_default_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
            pip_default_desc.depth.write_enabled = enableDepthWrite;

            pip = makePipeline(pip_default_desc);
            if (pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_DEFAULT_NODEPTH){
            pip_default_desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
            pip_default_desc.depth.write_enabled = false;

            nodepth_pip = makePipeline(pip_default_desc);
            if (nodepth_pip.id == SG_INVALID_ID){
                return false;
            }
        }
    }

    // PIP_RTT (offscreen), PIP_RTT_INVERT (planar reflection) and the other PIP_RTT_* variants share
    // an identical pipeline except for winding, depth and blend state, so build the desc once.
    if (pipelines & ((int)PipelineType::PIP_RTT | (int)PipelineType::PIP_RTT_INVERT |
            (int)PipelineType::PIP_RTT_NODEPTH | (int)PipelineType::PIP_RTT_NOBLEND |
            (int)PipelineType::PIP_RTT_COLORONLY)){
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
        pip_rtt_desc.face_winding = getFaceWinding(rttWinding);
        if (enableFaceCulling){
            pip_rtt_desc.cull_mode = getCullMode(cullingMode);
        }

        if (pipelines & (int)PipelineType::PIP_RTT){
            rtt_pip = makePipeline(pip_rtt_desc);
            if (rtt_pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_RTT_NODEPTH){
            sg_pipeline_desc pip_rtt_nodepth_desc = pip_rtt_desc;
            pip_rtt_nodepth_desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
            pip_rtt_nodepth_desc.depth.write_enabled = false;

            rtt_nodepth_pip = makePipeline(pip_rtt_nodepth_desc);
            if (rtt_nodepth_pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_RTT_NOBLEND){
            sg_pipeline_desc pip_rtt_noblend_desc = pip_rtt_desc;
            pip_rtt_noblend_desc.colors[0].blend.enabled = false;

            rtt_noblend_pip = makePipeline(pip_rtt_noblend_desc);
            if (rtt_noblend_pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_RTT_COLORONLY){
            sg_pipeline_desc pip_rtt_coloronly_desc = pip_rtt_desc;
            pip_rtt_coloronly_desc.colors[0].blend.enabled = false;
            pip_rtt_coloronly_desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
            pip_rtt_coloronly_desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
            pip_rtt_coloronly_desc.depth.write_enabled = false;

            rtt_coloronly_pip = makePipeline(pip_rtt_coloronly_desc);
            if (rtt_coloronly_pip.id == SG_INVALID_ID){
                return false;
            }
        }

        if (pipelines & (int)PipelineType::PIP_RTT_INVERT){
            // planar reflection mirrors handedness, reversing winding once more
            WindingOrder invWinding = (rttWinding == WindingOrder::CCW) ? WindingOrder::CW : WindingOrder::CCW;
            pip_rtt_desc.face_winding = getFaceWinding(invWinding);
            rtt_invert_pip = makePipeline(pip_rtt_desc);
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
    }else if (pipType == PipelineType::PIP_ZPREPASS){
        selectedPipeline = zprepass_pip;
    }else if (pipType == PipelineType::PIP_ZPREPASS_RTT){
        selectedPipeline = zprepass_rtt_pip;
    }else if (pipType == PipelineType::PIP_SHADOW_DEPTH){
        selectedPipeline = shadow_depth_pip;
    }else if (pipType == PipelineType::PIP_GBUFFER){
        selectedPipeline = gbuffer_pip;
    }else if (pipType == PipelineType::PIP_RTT){
        selectedPipeline = rtt_pip;
    }else if (pipType == PipelineType::PIP_RTT_INVERT){
        selectedPipeline = rtt_invert_pip;
    }else if (pipType == PipelineType::PIP_DEFAULT_NODEPTH){
        selectedPipeline = nodepth_pip;
    }else if (pipType == PipelineType::PIP_RTT_NODEPTH){
        selectedPipeline = rtt_nodepth_pip;
    }else if (pipType == PipelineType::PIP_RTT_NOBLEND){
        selectedPipeline = rtt_noblend_pip;
    }else if (pipType == PipelineType::PIP_RTT_COLORONLY){
        selectedPipeline = rtt_coloronly_pip;
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
        destroyPipeline(pip);
        destroyPipeline(depth_pip);
        destroyPipeline(shadow_depth_pip);
        destroyPipeline(rtt_pip);
        destroyPipeline(rtt_invert_pip);
        destroyPipeline(gbuffer_pip);
        destroyPipeline(nodepth_pip);
        destroyPipeline(rtt_nodepth_pip);
        destroyPipeline(rtt_noblend_pip);
        destroyPipeline(rtt_coloronly_pip);
        destroyPipeline(zprepass_pip);
        destroyPipeline(zprepass_rtt_pip);
    }

    pip.id = SG_INVALID_ID;
    depth_pip.id = SG_INVALID_ID;
    shadow_depth_pip.id = SG_INVALID_ID;
    rtt_pip.id = SG_INVALID_ID;
    rtt_invert_pip.id = SG_INVALID_ID;
    gbuffer_pip.id = SG_INVALID_ID;
    nodepth_pip.id = SG_INVALID_ID;
    rtt_nodepth_pip.id = SG_INVALID_ID;
    rtt_noblend_pip.id = SG_INVALID_ID;
    rtt_coloronly_pip.id = SG_INVALID_ID;
    zprepass_pip.id = SG_INVALID_ID;
    zprepass_rtt_pip.id = SG_INVALID_ID;
    bind = {};
    pipeline_desc = {};
    bindSlotIndex = 0;
}
