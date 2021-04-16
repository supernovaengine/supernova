#include "SokolShader.h"

#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include "shaders/shaders.h"
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

bool SokolShader::createShader(ShaderType shaderType){
    if (shaderType == ShaderType::MESH){
        shader = sg_make_shader(mesh_shader_desc(sg_query_backend()));
    }else if (shaderType == ShaderType::MESH_PBR_UNLIT){
        shader = sg_make_shader(meshPBR_unlit_shader_desc(sg_query_backend()));
    }else if (shaderType == ShaderType::MESH_PBR){
        shader = sg_make_shader(meshPBR_shader_desc(sg_query_backend()));
    }else if (shaderType == ShaderType::MESH_PBR_NONMAP_NOTAN){
        shader = sg_make_shader(meshPBR_noNmap_noTan_shader_desc(sg_query_backend()));
    }else if (shaderType == ShaderType::SKYBOX){
        shader = sg_make_shader(skybox_shader_desc(sg_query_backend()));
    }

    if (shader.id != SG_INVALID_ID)
        return true;

    return false;
}

void SokolShader::destroyShader(){
    if (shader.id != SG_INVALID_ID && sg_isvalid()){
        sg_destroy_shader(shader);
    }
    
    shader.id = SG_INVALID_ID;
}

sg_shader SokolShader::get(){
    return shader;
}