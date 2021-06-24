//
// (c) 2021 Eduardo Doria.
//

#include "ShaderData.h"

#include "Log.h"

#include <cstring>

using namespace Supernova;

ShaderData::ShaderData(){

}

ShaderData::~ShaderData(){
    releaseSourceData();
}

ShaderData::ShaderData(const ShaderData& d){
    this->lang = d.lang;
    this->profileVersion = d.profileVersion;
    this->stages = d.stages;

    for (int i = 0; i < stages.size(); i++){
        this->stages[i].bytecode.data = new unsigned char[this->stages[i].bytecode.size];
        memcpy(this->stages[i].bytecode.data, d.stages[i].bytecode.data, this->stages[i].bytecode.size);
    }
}

ShaderData& ShaderData::operator = (const ShaderData& d){
    this->lang = d.lang;
    this->profileVersion = d.profileVersion;
    this->stages = d.stages;

    for (int i = 0; i < stages.size(); i++){
        this->stages[i].bytecode.data = new unsigned char[this->stages[i].bytecode.size];
        memcpy(this->stages[i].bytecode.data, d.stages[i].bytecode.data, this->stages[i].bytecode.size);
    }

    return *this;
}

int ShaderData::getAttrIndex(AttributeType type){
    std::string attrstr;
    
    if (type == AttributeType::POSITION){
        attrstr = "a_position";
    }else if (type == AttributeType::TEXCOORD1){
        attrstr = "a_texcoord1";
    }else if (type == AttributeType::NORMAL){
        attrstr = "a_normal";
    }else if (type == AttributeType::TANGENT){
        attrstr = "a_tangent";
    }else if (type == AttributeType::COLOR){
        attrstr = "a_color";
    }

    if (attrstr.empty()){
        Log::Error("Trying to get a not mapped attribute");
        return -1;
    }

    int vsIndex = -1;
    for (int s = 0; s < stages.size(); s++){
        if (stages[s].type == ShaderStageType::VERTEX)
            vsIndex = s;
    }

    int attrIndex = -1;
    if (vsIndex != -1){
        for (int a = 0; a < stages[vsIndex].attributes.size(); a++){
            if (stages[vsIndex].attributes[a].name == attrstr)
                attrIndex = a;
        }
    }

    return attrIndex;
}

int ShaderData::getUniformIndex(UniformType type, ShaderStageType stage){
    std::string ustr;
    
    if (type == UniformType::PBR_VS_PARAMS){
        ustr = "u_vs_pbrParams";
    }else if (type == UniformType::PBR_FS_PARAMS){
        ustr = "u_fs_pbrParams";
    }else if (type == UniformType::VS_LIGHTING){
        ustr = "u_vs_lighting";
    }else if (type == UniformType::FS_LIGHTING){
        ustr = "u_fs_lighting";
    }else if (type == UniformType::VIEWPROJECTIONSKY){
        ustr = "u_vsSkyParams";
    }else if (type == UniformType::DEPTH_VS_PARAMS){
        ustr = "u_vs_depthParams";
    }

    if (ustr.empty()){
        Log::Error("Trying to get a not mapped uniform");
        return -1;
    }

    int sIndex = -1;
    for (int s = 0; s < stages.size(); s++){
        if (stages[s].type == stage)
            sIndex = s;
    }

    int uniIndex = -1;
    if (sIndex != -1){
        for (int u = 0; u < stages[sIndex].uniforms.size(); u++){
            if (stages[sIndex].uniforms[u].name == ustr)
                uniIndex = u;
        }
    }

    return uniIndex;
}

int ShaderData::getTextureIndex(TextureShaderType type, ShaderStageType stage){
    std::string texstr;
    
    if (type == TextureShaderType::BASECOLOR){
        texstr = "u_baseColorTexture";
    }else if (type == TextureShaderType::EMISSIVE){
        texstr = "u_emissiveTexture";
    }else if (type == TextureShaderType::METALLICROUGHNESS){
        texstr = "u_metallicRoughnessTexture";
    }else if (type == TextureShaderType::OCCULSION){
        texstr = "u_occlusionTexture";
    }else if (type == TextureShaderType::NORMAL){
        texstr = "u_normalTexture";
    }else if (type == TextureShaderType::SHADOWMAP1){
        texstr = "u_shadowMap1";
    }else if (type == TextureShaderType::SHADOWMAP2){
        texstr = "u_shadowMap2";
    }else if (type == TextureShaderType::SHADOWMAP3){
        texstr = "u_shadowMap3";
    }else if (type == TextureShaderType::SHADOWMAP4){
        texstr = "u_shadowMap4";
    }else if (type == TextureShaderType::SHADOWMAP5){
        texstr = "u_shadowMap5";
    }else if (type == TextureShaderType::SHADOWMAP6){
        texstr = "u_shadowMap6";
    }else if (type == TextureShaderType::SHADOWMAP7){
        texstr = "u_shadowMap7";
    }else if (type == TextureShaderType::SHADOWMAP8){
        texstr = "u_shadowMap8";
    }else if (type == TextureShaderType::SKYCUBE){
        texstr = "u_skyTexture";
    }

    if (texstr.empty()){
        Log::Error("Trying to get a not mapped texture");
        return -1;
    }

    int sIndex = -1;
    for (int s = 0; s < stages.size(); s++){
        if (stages[s].type == stage)
            sIndex = s;
    }

    int texIndex = -1;
    if (sIndex != -1){
        for (int t = 0; t < stages[sIndex].textures.size(); t++){
            if (stages[sIndex].textures[t].name == texstr)
                texIndex = t;
        }
    }

    return texIndex;
}

void ShaderData::releaseSourceData(){
    for (int i = 0; i < stages.size(); i++){
        if (stages[i].bytecode.data)
            delete stages[i].bytecode.data;
        stages[i].bytecode.data = NULL;
        stages[i].bytecode.size = 0;
        stages[i].source.clear();
    }
}