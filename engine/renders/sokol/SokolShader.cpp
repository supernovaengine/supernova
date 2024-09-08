//
// (c) 2024 Eduardo Doria.
//

#include "SokolShader.h"

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "Log.h"
#include "SokolCmdQueue.h"
#include "render/ShaderRender.h"
#include "Engine.h"

using namespace Supernova;

SokolShader::SokolShader(){
    shader.id = SG_INVALID_ID;
}

SokolShader::SokolShader(const SokolShader& rhs) : shader(rhs.shader) {}

SokolShader& SokolShader::operator=(const SokolShader& rhs){ 
    shader = rhs.shader; 
    return *this;
}

int SokolShader::roundup(int val, int round_to) {
    return (val + (round_to - 1)) & ~(round_to - 1);
}

sg_uniform_type SokolShader::uniformToSokolType(ShaderUniformType type){
    if (type == ShaderUniformType::FLOAT){
        return SG_UNIFORMTYPE_FLOAT;
    }else if (type == ShaderUniformType::FLOAT2){
        return SG_UNIFORMTYPE_FLOAT2;
    }else if (type == ShaderUniformType::FLOAT3){
        return SG_UNIFORMTYPE_FLOAT3;
    }else if (type == ShaderUniformType::FLOAT4){
        return SG_UNIFORMTYPE_FLOAT4;
    }else if (type == ShaderUniformType::INT){
        return SG_UNIFORMTYPE_INT;
    }else if (type == ShaderUniformType::INT2){
        return SG_UNIFORMTYPE_INT2;
    }else if (type == ShaderUniformType::INT3){
        return SG_UNIFORMTYPE_INT3;
    }else if (type == ShaderUniformType::INT4){
        return SG_UNIFORMTYPE_INT4;
    }else if (type == ShaderUniformType::MAT3){
        Log::error("Sokol cannot support MAT3 uniform type");
    }else if (type == ShaderUniformType::MAT4){
        return SG_UNIFORMTYPE_MAT4;
    }

    return SG_UNIFORMTYPE_INVALID;
}

sg_uniform_type SokolShader::flattenedUniformToSokolType(ShaderUniformType type){
    if (type == ShaderUniformType::FLOAT){
        return SG_UNIFORMTYPE_FLOAT4;
    }else if (type == ShaderUniformType::FLOAT2){
        return SG_UNIFORMTYPE_FLOAT4;
    }else if (type == ShaderUniformType::FLOAT3){
        return SG_UNIFORMTYPE_FLOAT4;
    }else if (type == ShaderUniformType::FLOAT4){
        return SG_UNIFORMTYPE_FLOAT4;
    }else if (type == ShaderUniformType::INT){
        return SG_UNIFORMTYPE_INT4;
    }else if (type == ShaderUniformType::INT2){
        return SG_UNIFORMTYPE_INT4;
    }else if (type == ShaderUniformType::INT3){
        return SG_UNIFORMTYPE_INT4;
    }else if (type == ShaderUniformType::INT4){
        return SG_UNIFORMTYPE_INT4;
    }else if (type == ShaderUniformType::MAT3){
        return SG_UNIFORMTYPE_FLOAT4;
    }else if (type == ShaderUniformType::MAT4){
        return SG_UNIFORMTYPE_FLOAT4;
    }

    return SG_UNIFORMTYPE_INVALID;
}

sg_image_sample_type SokolShader::textureSamplerToSokolType(TextureSamplerType type){
    if (type == TextureSamplerType::FLOAT){
        return SG_IMAGESAMPLETYPE_FLOAT;
    }else if (type == TextureSamplerType::UINT){
        return SG_IMAGESAMPLETYPE_UINT;
    }else if (type == TextureSamplerType::SINT){
        return SG_IMAGESAMPLETYPE_SINT;
    }

    return _SG_IMAGESAMPLETYPE_DEFAULT;
}

sg_image_type SokolShader::textureToSokolType(TextureType type){
    if (type == TextureType::TEXTURE_2D){
        return SG_IMAGETYPE_2D;
    }else if (type == TextureType::TEXTURE_3D){
        return SG_IMAGETYPE_3D;
    }else if (type == TextureType::TEXTURE_CUBE){
        return SG_IMAGETYPE_CUBE;
    }else if (type == TextureType::TEXTURE_ARRAY){
        return SG_IMAGETYPE_ARRAY;
    }

    return _SG_IMAGETYPE_DEFAULT;
}

sg_sampler_type SokolShader::samplerToSokolType(SamplerType type){
    if (type == SamplerType::COMPARISON){
        return SG_SAMPLERTYPE_COMPARISON;
    }else if (type == SamplerType::FILTERING){
        return SG_SAMPLERTYPE_FILTERING;
    }

    return _SG_SAMPLERTYPE_DEFAULT;
}

bool SokolShader::createShader(ShaderData& shaderData){
    sg_shader_desc shader_desc = {0};

    for (int i = 0; i < shaderData.stages.size(); i++){
        const ShaderStage* stage =  &shaderData.stages[i];
        sg_shader_stage_desc* stage_desc = NULL;

        if (stage->type == ShaderStageType::VERTEX){
            stage_desc = &shader_desc.vs;
            stage_desc->d3d11_target = "vs_5_0";
        }else if (stage->type == ShaderStageType::FRAGMENT){
            stage_desc = &shader_desc.fs;
            stage_desc->d3d11_target = "ps_5_0";
        }

        if (shaderData.lang == ShaderLang::MSL){
            stage_desc->entry = "main0";
        }

        if (stage->bytecode.data) {
            stage_desc->bytecode = {(const uint8_t*)stage->bytecode.data, stage->bytecode.size};
        } else if (!stage->source.empty()) {
            stage_desc->source = stage->source.c_str();
        }

        if (SG_MAX_VERTEX_ATTRIBUTES < stage->attributes.size()){
            Log::error("Number of attributes of shader is bigger than SG_MAX_VERTEX_ATTRIBUTES");
        }else{
            // attributes
            for (int a = 0; a < stage->attributes.size(); a++) {
                int location = stage->attributes[a].location;
                if (location >= 0){
                    shader_desc.attrs[location].name = stage->attributes[a].name.c_str();
                    shader_desc.attrs[location].sem_name = stage->attributes[a].semanticName.c_str();
                    shader_desc.attrs[location].sem_index = stage->attributes[a].semanticIndex;
                }
            }
        }
    
        // uniform blocks
        for (int ub = 0; ub < stage->uniformblocks.size(); ub++) {
            sg_shader_uniform_block_desc* ubdesc = &stage_desc->uniform_blocks[stage->uniformblocks[ub].binding];
            ubdesc->size = roundup(stage->uniformblocks[ub].sizeBytes, 16);
            ubdesc->layout = SG_UNIFORMLAYOUT_STD140;
            // GL/GLES always flatten UBs if same type inside to not declare individual uniforms and use only one glUniform4fv call
            if ((shaderData.lang == ShaderLang::GLSL) && (stage->uniformblocks[ub].uniforms.size() > 0)) {
                if (stage->uniformblocks[ub].flattened){
                    ubdesc->uniforms[0].name = stage->uniformblocks[ub].name.c_str();
                    ubdesc->uniforms[0].type = flattenedUniformToSokolType(stage->uniformblocks[ub].uniforms[0].type);
                    ubdesc->uniforms[0].array_count = ubdesc->size / 16;
                }else{
                    for (int u = 0; u < (int)stage->uniformblocks[ub].uniforms.size(); u++) {
                        ubdesc->uniforms[u].name = stage->uniformblocks[ub].uniforms[u].name.c_str();
                        ubdesc->uniforms[u].type = uniformToSokolType(stage->uniformblocks[ub].uniforms[u].type);
                        ubdesc->uniforms[u].array_count = stage->uniformblocks[ub].uniforms[u].arrayCount;
                    }
                }
            }
        }

        // storage buffers
        for (int sb = 0; sb < stage->storagebuffers.size(); sb++) {
            sg_shader_storage_buffer_desc* sbdesc = &stage_desc->storage_buffers[stage->storagebuffers[sb].binding];

            sbdesc->used = true;
            sbdesc->readonly = stage->storagebuffers[sb].readonly;
        }

        // textures
        for (int t = 0; t < stage->textures.size(); t++) {
            sg_shader_image_desc* img = &stage_desc->images[stage->textures[t].binding];

            img->used = true;
            img->image_type = textureToSokolType(stage->textures[t].type);
            img->sample_type = textureSamplerToSokolType(stage->textures[t].samplerType);
        }

        // samplers
        for (int s = 0; s < stage->samplers.size(); s++) {
            sg_shader_sampler_desc* sampler = &stage_desc->samplers[stage->samplers[s].binding];

            sampler->used = true;
            sampler->sampler_type = samplerToSokolType(stage->samplers[s].type);
        }

        // texture sampler pair
        for (int ts = 0; ts < stage->textureSamplersPair.size(); ts++) {
            sg_shader_image_sampler_pair_desc* imgsamplerpair = &stage_desc->image_sampler_pairs[stage->textureSamplersPair[ts].binding];

            // get texture index
            int texIndex = -1;
            for (int t = 0; t < stage->textures.size(); t++){
                if (stage->textures[t].name == stage->textureSamplersPair[ts].textureName)
                    texIndex = stage->textures[t].binding;
            }

            // get sampler index
            int samIndex = -1;
            for (int s = 0; s < stage->samplers.size(); s++){
                if (stage->samplers[s].name == stage->textureSamplersPair[ts].samplerName)
                    samIndex = stage->samplers[s].binding;
            }

            imgsamplerpair->used = true;
            imgsamplerpair->image_slot = texIndex;
            imgsamplerpair->sampler_slot = samIndex;
            imgsamplerpair->glsl_name = stage->textureSamplersPair[ts].name.c_str();
        }
    }

    if (Engine::isAsyncThread()){
        shader = SokolCmdQueue::add_command_make_shader(shader_desc);
    }else{
        shader = sg_make_shader(shader_desc);
    }

    return isCreated();
}

void SokolShader::destroyShader(){
    if (shader.id != SG_INVALID_ID && sg_isvalid()){
        if (Engine::isAsyncThread()){
            SokolCmdQueue::add_command_destroy_shader(shader);
        }else{
            sg_destroy_shader(shader);
        }
    }
    
    shader.id = SG_INVALID_ID;
}

bool SokolShader::isCreated(){
    if (shader.id != SG_INVALID_ID)
        return true;

    return false;
}

sg_shader SokolShader::get(){
    return shader;
}