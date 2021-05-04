//
// (c) 2021 Eduardo Doria.
// Based on a work of Sepehr Taghdisian (https://github.com/septag/glslcc)
//

#include "SGSReader.h"

#include "File.h"
#include "Log.h"

#define makefourcc(_a, _b, _c, _d) (((uint32_t)(_a) | ((uint32_t)(_b) << 8) | ((uint32_t)(_c) << 16) | ((uint32_t)(_d) << 24)))

//----------Begin SGS format---------------
#pragma pack(push, 1)

#define SGS_CHUNK           makefourcc('S', 'G', 'S', ' ')
#define SGS_CHUNK_STAG      makefourcc('S', 'T', 'A', 'G')
#define SGS_CHUNK_REFL      makefourcc('R', 'E', 'F', 'L')
#define SGS_CHUNK_CODE      makefourcc('C', 'O', 'D', 'E')
#define SGS_CHUNK_DATA      makefourcc('D', 'A', 'T', 'A')

#define SGS_LANG_GLES makefourcc('G', 'L', 'E', 'S')
#define SGS_LANG_HLSL makefourcc('H', 'L', 'S', 'L')
#define SGS_LANG_GLSL makefourcc('G', 'L', 'S', 'L')
#define SGS_LANG_MSL  makefourcc('M', 'S', 'L', ' ')

#define SGS_VERTEXFORMAT_FLOAT      makefourcc('F', 'L', 'T', '1')
#define SGS_VERTEXFORMAT_FLOAT2     makefourcc('F', 'L', 'T', '2')
#define SGS_VERTEXFORMAT_FLOAT3     makefourcc('F', 'L', 'T', '3')
#define SGS_VERTEXFORMAT_FLOAT4     makefourcc('F', 'L', 'T', '4')
#define SGS_VERTEXFORMAT_INT        makefourcc('I', 'N', 'T', '1')
#define SGS_VERTEXFORMAT_INT2       makefourcc('I', 'N', 'T', '2')
#define SGS_VERTEXFORMAT_INT3       makefourcc('I', 'N', 'T', '3')
#define SGS_VERTEXFORMAT_INT4       makefourcc('I', 'N', 'T', '4')

#define SGS_STAGE_VERTEX            makefourcc('V', 'E', 'R', 'T')
#define SGS_STAGE_FRAGMENT          makefourcc('F', 'R', 'A', 'G')
#define SGS_STAGE_COMPUTE           makefourcc('C', 'O', 'M', 'P')

#define SGS_IMAGEDIM_1D            makefourcc('1', 'D', ' ', ' ')
#define SGS_IMAGEDIM_2D            makefourcc('2', 'D', ' ', ' ')
#define SGS_IMAGEDIM_3D            makefourcc('3', 'D', ' ', ' ')
#define SGS_IMAGEDIM_CUBE          makefourcc('C', 'U', 'B', 'E')
#define SGS_IMAGEDIM_RECT          makefourcc('R', 'E', 'C', 'T')
#define SGS_IMAGEDIM_BUFFER        makefourcc('B', 'U', 'F', 'F')
#define SGS_IMAGEDIM_SUBPASS       makefourcc('S', 'U', 'B', 'P')

// SGS chunk
struct sgs_chunk {
    uint32_t lang;
    uint32_t profile_ver;
};

// REFL
struct sgs_chunk_refl {
    char     name[32];
    uint32_t num_inputs;
    uint32_t num_textures;
    uint32_t num_uniform_buffers;
    uint32_t num_storage_images;
    uint32_t num_storage_buffers;
    uint16_t flatten_ubos;
    uint16_t debug_info;

    // inputs: sgs_refl_input[num_inputs]
    // uniform-buffers: sgs_refl_uniformbuffer[num_uniform_buffers]
    // textures: sgs_refl_texture[num_textures]
    // storage_images: sgs_refl_texture[num_storage_images]
    // storage_buffers: sgs_refl_buffer[num_storage_buffers]
};

struct sgs_refl_input {
    char     name[32];
    int32_t  loc;
    char     semantic[32];
    uint32_t semantic_index;
    uint32_t format;
};

struct sgs_refl_texture {
    char     name[32];
    int32_t  binding;
    uint32_t image_dim;
    uint8_t  multisample;
    uint8_t  is_array;
};

struct sgs_refl_buffer {
    char    name[32];
    int32_t binding;
    uint32_t size_bytes;
    uint32_t array_stride;
};

struct sgs_refl_uniformbuffer {
    char     name[32];
    int32_t  binding;
    uint32_t size_bytes;
    uint16_t array_size;
};

#pragma pack(pop)
//----------End SGS format---------------


using namespace Supernova;


SGSReader::SGSReader(){
    vertexShader.bytecode.data = NULL;
    fragmentShader.bytecode.data = NULL;
}

SGSReader::~SGSReader(){
    if (vertexShader.bytecode.data)
        delete vertexShader.bytecode.data;

    if (fragmentShader.bytecode.data)
        delete fragmentShader.bytecode.data;
}

bool SGSReader::read(std::string filepath){
    File file;

    if (file.open(filepath.c_str()) != FileErrors::NO_ERROR){
        Log::Error("Cannot open sgs file: %s", filepath.c_str());
        return false;
    }

    uint32_t sgs = file.read32();
    if (sgs != SGS_CHUNK) {
        Log::Error("Invalid sgs file format: %s", filepath.c_str());
        return false;
    }

    file.seek(file.pos()+sizeof(uint32_t));

    sgs_chunk sinfo;
    file.read((unsigned char*)&sinfo, sizeof(sinfo));

    profileVersion = sinfo.profile_ver;

    if (sinfo.lang == SGS_LANG_GLSL){
        lang = ShaderLang::GLSL;
    }else if (sinfo.lang == SGS_LANG_GLES){
        lang = ShaderLang::GLES;
    }else if (sinfo.lang == SGS_LANG_HLSL){
        lang = ShaderLang::HLSL;
    }else if (sinfo.lang == SGS_LANG_MSL){
        lang = ShaderLang::MSL;
    }

    uint32_t stag = file.read32();
    uint32_t stagSize = file.read32();

    ShaderStage* shaderStage;

    while (stag == SGS_CHUNK_STAG){
        uint32_t stagPos = file.pos();

        uint32_t stageType = file.read32();

        if (stageType == SGS_STAGE_VERTEX) {
            shaderStage = &vertexShader;
        } else if (stageType == SGS_STAGE_FRAGMENT) {
            shaderStage = &fragmentShader;
        } else if (stageType == SGS_STAGE_COMPUTE) {
            Log::Error("Stage compute is not supported: %s", filepath.c_str());
        } else {
            Log::Error("Stage not implemented: %s", filepath.c_str());
            return false;
        }

        uint32_t code = file.read32();
        uint32_t codeSize = file.read32();

        if (code == SGS_CHUNK_CODE){
            shaderStage->source = file.readString(codeSize);
        }else if (code == SGS_CHUNK_DATA){
            shaderStage->bytecode.data = new unsigned char[codeSize];
            shaderStage->bytecode.size = codeSize;
            file.read((unsigned char*)&shaderStage->bytecode.data, codeSize);
        }

        uint32_t refl = file.read32();
        uint32_t reflSize = file.read32();

        if (refl == SGS_CHUNK_REFL){
            sgs_chunk_refl refl_chunk;
            file.read((unsigned char*)&refl_chunk, sizeof(refl_chunk));

            shaderStage->name = std::string(refl_chunk.name);
            shaderStage->flattenUBOs = refl_chunk.flatten_ubos ? true : false;

            for (uint32_t i = 0; i < refl_chunk.num_inputs; i++) {
                sgs_refl_input in;
                file.read((unsigned char*)&in, sizeof(in));

                ShaderAttr attr;
                attr.location = in.loc;
                attr.name = std::string(in.name);
                attr.semanticName = std::string(in.semantic);
                attr.semanticIndex = in.semantic_index;
                if (in.format == SGS_VERTEXFORMAT_FLOAT){
                    attr.format = ShaderVertexFormat::FLOAT;
                }else if (in.format == SGS_VERTEXFORMAT_FLOAT2){
                    attr.format = ShaderVertexFormat::FLOAT2;
                }else if (in.format == SGS_VERTEXFORMAT_FLOAT3){
                    attr.format = ShaderVertexFormat::FLOAT3;
                }else if (in.format == SGS_VERTEXFORMAT_FLOAT4){
                    attr.format = ShaderVertexFormat::FLOAT4;
                }else if (in.format == SGS_VERTEXFORMAT_INT){
                    attr.format = ShaderVertexFormat::INT;
                }else if (in.format == SGS_VERTEXFORMAT_INT2){
                    attr.format = ShaderVertexFormat::INT2;
                }else if (in.format == SGS_VERTEXFORMAT_INT3){
                    attr.format = ShaderVertexFormat::INT3;
                }else if (in.format == SGS_VERTEXFORMAT_INT4){
                    attr.format = ShaderVertexFormat::INT4;
                }
                shaderStage->attributes.push_back(attr);
            }

            for (uint32_t i = 0; i < refl_chunk.num_uniform_buffers; i++) {
                sgs_refl_uniformbuffer u;
                file.read((unsigned char*)&u, sizeof(u));

                ShaderUniform uniform;
                uniform.name = std::string(u.name);
                uniform.binding = u.binding;
                uniform.sizeBytes = u.size_bytes;
                uniform.arraySize = u.array_size;
                shaderStage->uniforms.push_back(uniform);
            }

            for (uint32_t i = 0; i < refl_chunk.num_textures; i++) {
                sgs_refl_texture t;
                file.read((unsigned char*)&t, sizeof(t));

                ShaderTexture texture;
                texture.name = std::string(t.name);
                texture.binding = t.binding;
                texture.multisample = t.multisample ? true : false;

                bool array = t.is_array ? true : false;
                if (array && t.image_dim == SGS_IMAGEDIM_2D){
                    texture.type = TextureType::TEXTURE_ARRAY;
                }else if (!array){
                    if (t.image_dim == SGS_IMAGEDIM_2D)
                        texture.type = TextureType::TEXTURE_2D;
                    else if (t.image_dim == SGS_IMAGEDIM_3D)
                        texture.type = TextureType::TEXTURE_3D;
                    else if (t.image_dim == SGS_IMAGEDIM_CUBE)
                        texture.type = TextureType::TEXTURE_CUBE;
                }

                shaderStage->textures.push_back(texture);
            }

            //for (uint32_t i = 0; i < refl_chunk.num_storage_images; i++) {
            //    sgs_refl_texture img;
            //    file.read((unsigned char*)&img, sizeof(img));
            //}

            //for (uint32_t i = 0; i < refl_chunk.num_storage_buffers; i++) {
            //    sgs_refl_buffer b;
            //    file.read((unsigned char*)&b, sizeof(b));
            //}
        }

        //Next STAG
        file.seek(stagPos + stagSize);
        stag = file.read32();
        stagSize = file.read32();
    }

    return true;
}

ShaderLang SGSReader::getLang(){
    return lang;
}

unsigned int SGSReader::getProfileVer(){
    return profileVersion;
}

ShaderStage& SGSReader::getVertexStage(){
    return vertexShader;
}

ShaderStage& SGSReader::getFragmentStage(){
    return fragmentShader;
}