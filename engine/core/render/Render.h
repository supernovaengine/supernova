#ifndef Render_h
#define Render_h

#define S_TEXTURESAMPLER_DIFFUSE 1
#define S_TEXTURESAMPLER_SHADOWMAP2D 2
#define S_TEXTURESAMPLER_SHADOWMAPCUBE 3
#define S_TEXTURESAMPLER_HEIGHTDATA 4
#define S_TEXTURESAMPLER_BLENDMAP 5
#define S_TEXTURESAMPLER_TERRAINDETAIL 6

#define S_PROGRAM_USE_FOG  1 << 0
#define S_PROGRAM_USE_TEXCOORD  1 << 1
#define S_PROGRAM_USE_TEXRECT  1 << 2
#define S_PROGRAM_USE_TEXCUBE  1 << 3
#define S_PROGRAM_USE_SKINNING  1 << 4
#define S_PROGRAM_USE_MORPHTARGET  1 << 5
#define S_PROGRAM_USE_MORPHNORMAL  1 << 6
#define S_PROGRAM_IS_SKY  1 << 7
#define S_PROGRAM_IS_TEXT  1 << 8
#define S_PROGRAM_IS_TERRAIN  1 << 9

namespace Supernova{

    enum class PrimitiveType{
        TRIANGLES,
        TRIANGLE_STRIP,
        POINTS,
        LINES
    };

    enum class BufferType{
        VERTEX_BUFFER,
        INDEX_BUFFER
    };

    enum class BufferUsage{ //see sokol_gfx.h for details
        IMMUTABLE,
        DYNAMIC,
        STREAM
    };

    enum class ShaderType{
        POINTS,
        LINES, // Not used yet
        MESH,
        SKYBOX,
        DEPTH,
        UI
    };

    enum class AttributeType{
        INDEX,
        POSITION,
        TEXCOORD1,
        NORMAL,
        TANGENT,
        COLOR,
        POINTSIZE,
        POINTROTATION,
        TEXTURERECT,
        BONEWEIGHTS,
        BONEIDS,
        MORPHTARGET0,
        MORPHTARGET1,
        MORPHTARGET2,
        MORPHTARGET3,
        MORPHTARGET4,
        MORPHTARGET5,
        MORPHTARGET6,
        MORPHTARGET7,
        MORPHNORMAL0,
        MORPHNORMAL1,
        MORPHNORMAL2,
        MORPHNORMAL3,
        MORPHTANGENT0,
        MORPHTANGENT1
    };

    enum class AttributeDataType{
        BYTE, //int8_t
        UNSIGNED_BYTE, //uint8_t
        SHORT, //int16_t
        UNSIGNED_SHORT, //uint16_t
        INT, //int32_t
        UNSIGNED_INT, //uint32_t
        FLOAT
    };

    enum class UniformBlockType{
        PBR_VS_PARAMS,
        MATERIAL,
        PBR_FS_PARAMS,
        FS_LIGHTING,
        FS_FOG,
        VS_SHADOWS,
        FS_SHADOWS,
        SKY_VS_PARAMS,
        SKY_FS_PARAMS,
        DEPTH_VS_PARAMS,
        SPRITE_VS_PARAMS,
        UI_VS_PARAMS,
        UI_FS_PARAMS,
        POINTS_VS_PARAMS,
        VS_SKINNING,
        DEPTH_VS_SKINNING,
        VS_MORPHTARGET,
        DEPTH_VS_MORPHTARGET,
        TERRAIN_VS_PARAMS,
        TERRAINNODE_VS_PARAMS
        /*
        TERRAINSIZE,
        TERRAINMAXHEIGHT,
        TERRAINRESOLUTION,
        TERRAINNODEPOS,
        TERRAINNODESIZE,
        TERRAINNODERANGE,
        TERRAINNODERESOLUTION,
        TERRAINTEXTUREBASETILES,
        TERRAINTEXTUREDETAILTILES,
        BLENDMAPCOLORINDEX
        */
    };

    enum class TextureShaderType{
        BASECOLOR,
        EMISSIVE,
        METALLICROUGHNESS,
        OCCULSION,
        NORMAL,
        SHADOWMAP1,
        SHADOWMAP2,
        SHADOWMAP3,
        SHADOWMAP4,
        SHADOWMAP5,
        SHADOWMAP6,
        SHADOWMAP7,
        SHADOWMAP8,
        SHADOWCUBEMAP1,
        SKYCUBE,
        UI,
        POINTS,
        HEIGHTMAP
        //SHADOWMAP2D
        //SHADOWMAPCUBE
        //HEIGHTDATA
        //BLENDMAP
        //TERRAINDETAIL
    };

    enum class TextureType {
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE,
        TEXTURE_ARRAY
    };

    enum class TextureSamplerType {
        SINT,
        UINT,
        FLOAT
    };

    enum class ColorFormat{
        RED,
        RGBA
    };

/*
    typedef struct shaderConfig{
        int programDefs;
        int numPointLights;
        int numSpotLights;
        int numDirLights;
        int numShadows2D;
        int numShadowsCube;
        int numBlendMapColors;
    } shaderConfig;
*/

    //-------Start shader definition--------
    enum class ShaderLang{
        GLSL,
        MSL,
        HLSL
    };

    enum class ShaderVertexType{
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4
    };

    enum class ShaderUniformType{
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4,
        MAT3,
        MAT4
    };

    enum class ShaderStageType{
        VERTEX,
        FRAGMENT
    };
    //-------End shader definition--------
}

#endif //Render_h