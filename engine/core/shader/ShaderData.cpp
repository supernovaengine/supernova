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
    this->version = d.version;
    this->stages = d.stages;

    for (int i = 0; i < stages.size(); i++){
        if (this->stages[i].bytecode.data){
            this->stages[i].bytecode.data = new unsigned char[this->stages[i].bytecode.size];
            memcpy(this->stages[i].bytecode.data, d.stages[i].bytecode.data, this->stages[i].bytecode.size);
        }
    }
}

ShaderData& ShaderData::operator = (const ShaderData& d){
    this->lang = d.lang;
    this->version = d.version;
    this->stages = d.stages;

    for (int i = 0; i < stages.size(); i++){
        if (this->stages[i].bytecode.data){
            this->stages[i].bytecode.data = new unsigned char[this->stages[i].bytecode.size];
            memcpy(this->stages[i].bytecode.data, d.stages[i].bytecode.data, this->stages[i].bytecode.size);
        }
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
    }else if (type == AttributeType::COLOR){
        attrstr = "a_color";
    }else if (type == AttributeType::POINTSIZE){
        attrstr = "a_pointsize";
    }else if (type == AttributeType::POINTROTATION){
        attrstr = "a_pointrotation";
    }else if (type == AttributeType::TEXTURERECT){
        attrstr = "a_texturerect";
    }else if (type == AttributeType::BONEIDS){
        attrstr = "a_boneIds";
    }else if (type == AttributeType::BONEWEIGHTS){
        attrstr = "a_boneWeights";
    }else if (type == AttributeType::MORPHTARGET0){
        attrstr = "a_morphTarget0";
    }else if (type == AttributeType::MORPHTARGET1){
        attrstr = "a_morphTarget1";
    }else if (type == AttributeType::MORPHTARGET2){
        attrstr = "a_morphTarget2";
    }else if (type == AttributeType::MORPHTARGET3){
        attrstr = "a_morphTarget3";
    }else if (type == AttributeType::MORPHTARGET4){
        attrstr = "a_morphTarget4";
    }else if (type == AttributeType::MORPHTARGET5){
        attrstr = "a_morphTarget5";
    }else if (type == AttributeType::MORPHTARGET6){
        attrstr = "a_morphTarget6";
    }else if (type == AttributeType::MORPHTARGET7){
        attrstr = "a_morphTarget7";
    }else if (type == AttributeType::MORPHNORMAL0){
        attrstr = "a_morphNormal0";
    }else if (type == AttributeType::MORPHNORMAL1){
        attrstr = "a_morphNormal1";
    }else if (type == AttributeType::MORPHNORMAL2){
        attrstr = "a_morphNormal2";
    }else if (type == AttributeType::MORPHNORMAL3){
        attrstr = "a_morphNormal3";
    }else if (type == AttributeType::MORPHTANGENT0){
        attrstr = "a_morphTangent0";
    }else if (type == AttributeType::MORPHTANGENT1){
        attrstr = "a_morphTangent1";
    }

    if (attrstr.empty()){
        Log::error("Trying to get a not mapped attribute");
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
                attrIndex = stages[vsIndex].attributes[a].location;
        }
    }

    return attrIndex;
}

int ShaderData::getUniformBlockIndex(UniformBlockType type, ShaderStageType stage){
    std::string ustr;
    
    if (type == UniformBlockType::PBR_VS_PARAMS){
        ustr = "u_vs_pbrParams";
    }else if (type == UniformBlockType::PBR_FS_PARAMS){
        ustr = "u_fs_pbrParams";
    }else if (type == UniformBlockType::FS_LIGHTING){
        ustr = "u_fs_lighting";
    }else if (type == UniformBlockType::FS_FOG){
        ustr = "u_fs_fog";
    }else if (type == UniformBlockType::VS_SHADOWS){
        ustr = "u_vs_shadows";
    }else if (type == UniformBlockType::FS_SHADOWS){
        ustr = "u_fs_shadows";
    }else if (type == UniformBlockType::SKY_VS_PARAMS){
        ustr = "u_vs_skyParams";
    }else if (type == UniformBlockType::SKY_FS_PARAMS){
        ustr = "u_fs_skyParams";
    }else if (type == UniformBlockType::DEPTH_VS_PARAMS){
        ustr = "u_vs_depthParams";
    }else if (type == UniformBlockType::UI_VS_PARAMS){
        ustr = "u_vs_uiParams";
    }else if (type == UniformBlockType::UI_FS_PARAMS){
        ustr = "u_fs_uiParams";
    }else if (type == UniformBlockType::SPRITE_VS_PARAMS){
        ustr = "u_vs_spriteParams";
    }else if (type == UniformBlockType::POINTS_VS_PARAMS){
        ustr = "u_vs_pointsParams";
    }else if (type == UniformBlockType::VS_SKINNING){
        ustr = "u_vs_skinning";
    }else if (type == UniformBlockType::DEPTH_VS_SKINNING){
        ustr = "u_vs_skinning";
    }else if (type == UniformBlockType::VS_MORPHTARGET){
        ustr = "u_vs_morphtarget";
    }else if (type == UniformBlockType::DEPTH_VS_MORPHTARGET){
        ustr = "u_vs_morphtarget";
    }else if (type == UniformBlockType::TERRAIN_VS_PARAMS){
        ustr = "u_vs_terrainParams";
    }else if (type == UniformBlockType::TERRAINNODE_VS_PARAMS){
        ustr = "u_vs_terrainNodeParams";
    }

    if (ustr.empty()){
        Log::error("Trying to get a not mapped uniform");
        return -1;
    }

    int sIndex = -1;
    for (int s = 0; s < stages.size(); s++){
        if (stages[s].type == stage)
            sIndex = s;
    }

    int uniIndex = -1;
    if (sIndex != -1){
        for (int u = 0; u < stages[sIndex].uniformblocks.size(); u++){
            if (stages[sIndex].uniformblocks[u].name == ustr)
                uniIndex = u;
        }
    }

    return uniIndex;
}

std::pair<int, int> ShaderData::getTextureIndex(TextureShaderType type, ShaderStageType stage){
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
    }else if (type == TextureShaderType::SHADOWCUBEMAP1){
        texstr = "u_shadowCubeMap1";
    }else if (type == TextureShaderType::SKYCUBE){
        texstr = "u_skyTexture";
    }else if (type == TextureShaderType::UI){
        texstr = "u_uiTexture";
    }else if (type == TextureShaderType::POINTS){
        texstr = "u_pointsTexture";
    }else if (type == TextureShaderType::HEIGHTMAP){
        texstr = "u_heightMap";
    }else if (type == TextureShaderType::BLENDMAP){
        texstr = "u_blendMap";
    }else if (type == TextureShaderType::TERRAINDETAIL_RED){
        texstr = "u_terrainDetailR";
    }else if (type == TextureShaderType::TERRAINDETAIL_GREEN){
        texstr = "u_terrainDetailG";
    }else if (type == TextureShaderType::TERRAINDETAIL_BLUE){
        texstr = "u_terrainDetailB";
    }

    if (texstr.empty()){
        Log::error("Trying to get a not mapped texture");
        return std::pair(-1, -1);
    }

    int sIndex = -1;
    for (int s = 0; s < stages.size(); s++){
        if (stages[s].type == stage)
            sIndex = s;
    }


    int texIndex = -1;
    int samIndex = -1;
    if (sIndex != -1){

        // get texture index
        for (int t = 0; t < stages[sIndex].textures.size(); t++){
            if (stages[sIndex].textures[t].name == texstr)
                texIndex = t;
        }

        // get sampler name by texture
        std::string samplerName;
        for (int ts = 0; ts < stages[sIndex].textureSamplersPair.size(); ts++){
            if (stages[sIndex].textureSamplersPair[ts].textureName == texstr){
                samplerName = stages[sIndex].textureSamplersPair[ts].samplerName;
            }
        }

        // get sampler index
        for (int s = 0; s < stages[sIndex].samplers.size(); s++){
            if (stages[sIndex].samplers[s].name == samplerName)
                samIndex = s;
        }
    }

    return std::pair(texIndex, samIndex);
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
