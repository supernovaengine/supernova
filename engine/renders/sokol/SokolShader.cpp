#include "SokolShader.h"

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "Log.h"

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

        // attributes
        for (int a = 0; a < stage->attributes.size(); a++) {
            int location = stage->attributes[a].location;
            if (location >= 0){
                shader_desc.attrs[location].name = stage->attributes[a].name.c_str();
                shader_desc.attrs[location].sem_name = stage->attributes[a].semanticName.c_str();
                shader_desc.attrs[location].sem_index = stage->attributes[a].semanticIndex;
            }
        }
    
        // uniform blocks
        for (int u = 0; u < stage->uniforms.size(); u++) {
            sg_shader_uniform_block_desc* ub = &stage_desc->uniform_blocks[stage->uniforms[u].binding];
            ub->size = roundup(stage->uniforms[u].sizeBytes, 16);
            // GL/GLES always flatten UBs to not declare individual uniforms and use only one glUniform4fv call
            if (shaderData.lang == ShaderLang::GLSL) {
                ub->uniforms[0].array_count = ub->size / 16;
                ub->uniforms[0].name = stage->uniforms[u].name.c_str();
                ub->uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
            }
        }

        //textures
        for (int t = 0; t < stage->textures.size(); t++) {
            sg_shader_image_desc* img = &stage_desc->images[stage->textures[t].binding];
            img->name = stage->textures[t].name.c_str();

            if (stage->textures[t].samplerType == TextureSamplerType::FLOAT){
                img->sampler_type = SG_SAMPLERTYPE_FLOAT;
            }else if (stage->textures[t].samplerType == TextureSamplerType::UINT){
                img->sampler_type = SG_SAMPLERTYPE_UINT;
            }else if (stage->textures[t].samplerType == TextureSamplerType::SINT){
                img->sampler_type = SG_SAMPLERTYPE_SINT;
            }

            if (stage->textures[t].type == TextureType::TEXTURE_2D){
                img->image_type = SG_IMAGETYPE_2D;
            }else if (stage->textures[t].type == TextureType::TEXTURE_3D){
                img->image_type = SG_IMAGETYPE_3D;
            }else if (stage->textures[t].type == TextureType::TEXTURE_CUBE){
                img->image_type = SG_IMAGETYPE_CUBE;
            }else if (stage->textures[t].type == TextureType::TEXTURE_ARRAY){
                img->image_type = SG_IMAGETYPE_ARRAY;
            }
        }
    }

    shader = sg_make_shader(&shader_desc);

    return isCreated();
}

void SokolShader::destroyShader(){
    if (shader.id != SG_INVALID_ID && sg_isvalid()){
        sg_destroy_shader(shader);
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