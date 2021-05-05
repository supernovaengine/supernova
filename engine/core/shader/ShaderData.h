//
// (c) 2021 Eduardo Doria.
//

#ifndef SHADERDATA_H
#define SHADERDATA_H

#include <string>
#include <vector>
#include "render/Render.h"

namespace Supernova {

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
        unsigned char* data = NULL;
        uint32_t size;
    };

    //TODO: Remove flattenUBOs that is used only in GL/GLES. Because Sokol 
    //      shader needs individual uniform names when using GL/GLES
    //      and SGS file format is not parsing them, only JSON format
    struct ShaderStage{
        ShaderStageType type;
        std::string name;
        bool flattenUBOs;

        std::string source;
        ShaderBinSource bytecode;

        std::vector<ShaderAttr> attributes;
        std::vector<ShaderUniform> uniforms;
        std::vector<ShaderTexture> textures;
    };

    class ShaderData {
    public:
        ShaderData();
        virtual ~ShaderData();

        ShaderLang lang;
        unsigned int profileVersion;

        std::vector<ShaderStage> stages;
    };

}


#endif //SHADERDATA_H