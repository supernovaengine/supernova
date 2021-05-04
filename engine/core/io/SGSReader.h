//
// (c) 2021 Eduardo Doria.
// Based on a work of Sepehr Taghdisian (https://github.com/septag/glslcc)
//

#ifndef SGSREADER_H
#define SGSREADER_H

#include <string>
#include <vector>
#include "render/Render.h"

namespace Supernova {

    enum class ShaderLang{
        GLSL,
        GLES,
        MSL,
        HLSL
    };

    enum class ShaderVertexFormat{
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4
    };

    struct ShaderAttr{
        std::string name;
        std::string semanticName;
        unsigned int semanticIndex;
        int location;
        ShaderVertexFormat format;
    };

    struct ShaderUniform {
        std::string name;
        int  binding;
        unsigned int sizeBytes;
        int arraySize;
    };

    struct ShaderTexture {
        std::string name;
        int binding;
        TextureType type;
        bool multisample;
    };

    struct ShaderBinSource{
        unsigned char* data;
        uint32_t size;
    };

    struct ShaderStage{
        std::string name;
        bool flattenUBOs;

        std::string source;
        ShaderBinSource bytecode;

        std::vector<ShaderAttr> attributes;
        std::vector<ShaderUniform> uniforms;
        std::vector<ShaderTexture> textures;
    };

    class SGSReader {
    private:
        ShaderLang lang;
        unsigned int profileVersion;
        ShaderStage vertexShader;
        ShaderStage fragmentShader;
        

    public:
        SGSReader();
        virtual ~SGSReader();

        bool read(std::string filepath);

        ShaderLang getLang();
        unsigned int getProfileVer();
        ShaderStage& getVertexStage();
        ShaderStage& getFragmentStage();
    };
}


#endif //SGSREADER_H
