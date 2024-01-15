//
// (c) 2021 Eduardo Doria.
//

#ifndef SHADERDATA_H
#define SHADERDATA_H

#include <string>
#include <vector>
#include <cstdint>
#include "render/Render.h"

namespace Supernova {

    struct ShaderAttr{
        std::string name;
        std::string semanticName;
        unsigned int semanticIndex;
        int location;
        ShaderVertexType type;
    };

    struct ShaderUniform {
        std::string name;
        ShaderUniformType type;
        unsigned int arrayCount;
        unsigned int offset;
    };

    struct ShaderUniformBlock {
        std::string name;
        std::string instName;
        int  set;
        int  binding;
        unsigned int sizeBytes;
        bool flattened;

        std::vector<ShaderUniform> uniforms;
    };

    struct ShaderTexture {
        std::string name;
        int set;
        int binding;
        TextureType type;
        TextureSamplerType samplerType;
    };

    struct ShaderSampler {
        std::string name;
        int set;
        int binding;
        SamplerType type;
    };

    struct ShaderTextureSampler {
        std::string name;
        std::string textureName;
        std::string samplerName;
        int binding;
    };

    struct ShaderBinSource{
        unsigned char* data = NULL;
        uint32_t size;
    };

    struct ShaderStage{
        ShaderStageType type;
        std::string name;

        std::string source;
        ShaderBinSource bytecode;

        std::vector<ShaderAttr> attributes;
        std::vector<ShaderUniformBlock> uniformblocks;
        std::vector<ShaderTexture> textures;
        std::vector<ShaderSampler> samplers;
        std::vector<ShaderTextureSampler> textureSamplersPair;
    };

    class ShaderData {
    public:
        ShaderLang lang;
        unsigned int version;
        bool es;
        std::vector<ShaderStage> stages;


        ShaderData();
        virtual ~ShaderData();

        ShaderData(const ShaderData& d);
        ShaderData& operator = (const ShaderData& d);

        int getAttrIndex(AttributeType type);
        int getUniformBlockIndex(UniformBlockType type, ShaderStageType stage);
        std::pair<int, int> getTextureIndex(TextureShaderType type, ShaderStageType stage);

        void releaseSourceData();
    };

}


#endif //SHADERDATA_H
