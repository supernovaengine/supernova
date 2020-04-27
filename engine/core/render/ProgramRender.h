#ifndef ProgramRender_h
#define ProgramRender_h

#define S_SHADER_POINTS 1
#define S_SHADER_LINES 2
#define S_SHADER_MESH 3
#define S_SHADER_DEPTH_RTT 4

#define S_INDEXATTRIBUTE 100
#define S_VERTEXATTRIBUTE_VERTICES 1
#define S_VERTEXATTRIBUTE_TEXTURECOORDS 2
#define S_VERTEXATTRIBUTE_NORMALS 3
#define S_VERTEXATTRIBUTE_BONEWEIGHTS 4
#define S_VERTEXATTRIBUTE_BONEIDS 5
#define S_VERTEXATTRIBUTE_POINTSIZES 6
#define S_VERTEXATTRIBUTE_POINTCOLORS 7
#define S_VERTEXATTRIBUTE_POINTROTATIONS 8
#define S_VERTEXATTRIBUTE_TEXTURERECTS 9
#define S_VERTEXATTRIBUTE_MORPHTARGET0 10
#define S_VERTEXATTRIBUTE_MORPHTARGET1 11
#define S_VERTEXATTRIBUTE_MORPHTARGET2 12
#define S_VERTEXATTRIBUTE_MORPHTARGET3 13
#define S_VERTEXATTRIBUTE_MORPHTARGET4 14
#define S_VERTEXATTRIBUTE_MORPHTARGET5 15
#define S_VERTEXATTRIBUTE_MORPHTARGET6 16
#define S_VERTEXATTRIBUTE_MORPHTARGET7 17
#define S_VERTEXATTRIBUTE_MORPHNORMAL0 18
#define S_VERTEXATTRIBUTE_MORPHNORMAL1 19
#define S_VERTEXATTRIBUTE_MORPHNORMAL2 20
#define S_VERTEXATTRIBUTE_MORPHNORMAL3 21

#define S_PROPERTY_MVPMATRIX 1
#define S_PROPERTY_MODELMATRIX 2
#define S_PROPERTY_NORMALMATRIX 3
#define S_PROPERTY_DEPTHVPMATRIX 4
#define S_PROPERTY_CAMERAPOS 5
#define S_PROPERTY_TEXTURERECT 6
#define S_PROPERTY_COLOR 7

#define S_PROPERTY_AMBIENTLIGHT 8

#define S_PROPERTY_POINTLIGHT_POS 9
#define S_PROPERTY_POINTLIGHT_POWER 10
#define S_PROPERTY_POINTLIGHT_COLOR 11
#define S_PROPERTY_POINTLIGHT_SHADOWIDX 12

#define S_PROPERTY_SPOTLIGHT_POS 13
#define S_PROPERTY_SPOTLIGHT_POWER 14
#define S_PROPERTY_SPOTLIGHT_COLOR 15
#define S_PROPERTY_SPOTLIGHT_TARGET 16
#define S_PROPERTY_SPOTLIGHT_CUTOFF 17
#define S_PROPERTY_SPOTLIGHT_OUTERCUTOFF 18
#define S_PROPERTY_SPOTLIGHT_SHADOWIDX 19

#define S_PROPERTY_DIRLIGHT_DIR 20
#define S_PROPERTY_DIRLIGHT_POWER 21
#define S_PROPERTY_DIRLIGHT_COLOR 22
#define S_PROPERTY_DIRLIGHT_SHADOWIDX 23

#define S_PROPERTY_FOG_MODE 24
#define S_PROPERTY_FOG_COLOR 25
#define S_PROPERTY_FOG_VISIBILITY 26
#define S_PROPERTY_FOG_DENSITY 27
#define S_PROPERTY_FOG_START 28
#define S_PROPERTY_FOG_END 29

#define S_PROPERTY_SHADOWLIGHT_POS 30
#define S_PROPERTY_SHADOWCAMERA_NEARFAR 31

#define S_PROPERTY_ISPOINTSHADOW 32

#define S_PROPERTY_SHADOWBIAS2D 33
#define S_PROPERTY_SHADOWBIASCUBE 34
#define S_PROPERTY_SHADOWCAMERA_NEARFAR2D 35
#define S_PROPERTY_SHADOWCAMERA_NEARFARCUBE 36

#define S_PROPERTY_NUMCASCADES2D 37

#define S_PROPERTY_BONESMATRIX 38

#define S_PROPERTY_MORPHWEIGHTS 39

#define S_PROPERTY_TERRAINSIZE 40
#define S_PROPERTY_TERRAINMAXHEIGHT 41
#define S_PROPERTY_TERRAINRESOLUTION 42
#define S_PROPERTY_TERRAINNODEPOS 43
#define S_PROPERTY_TERRAINNODESIZE 44
#define S_PROPERTY_TERRAINNODERANGE 45
#define S_PROPERTY_TERRAINNODERESOLUTION 46
#define S_PROPERTY_TERRAINTEXTUREBASETILES 47
#define S_PROPERTY_TERRAINTEXTUREDETAILTILES 48
#define S_PROPERTY_BLENDMAPCOLORINDEX 49

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

#include <string>
#include <unordered_map>
#include <regex>

namespace Supernova {

    class ProgramRender {
        
    private:
        
        typedef std::unordered_map< std::string, std::shared_ptr<ProgramRender> >::iterator it_type;
        static std::unordered_map< std::string, std::shared_ptr<ProgramRender> > programsRender;
        
        bool loaded;
        
        static ProgramRender* getProgramRender();
        static ProgramRender::it_type findToRemove();
        
    protected:

        int numPointLights;
        int numSpotLights;
        int numDirLights;
        int numShadows2D;
        int numShadowsCube;
        int numBlendMapColors;

        std::string regexReplace(std::string_view haystack, const std::regex& rx, std::function<std::string(const std::cmatch&)> f);
        std::string replaceAll(std::string source, const std::string from, const std::string to);
        std::string unrollLoops(std::string source);
        
        ProgramRender();
        
    public:
        
        virtual ~ProgramRender();
        
        static std::shared_ptr<ProgramRender> sharedInstance(std::string id);
        static void deleteUnused();
        
        bool isLoaded();

        int getNumPointLights();
        int getNumSpotLights();
        int getNumDirLights();
        int getNumShadows2D();
        int getNumShadowsCube();
        int getNumBlendMapColors();

        virtual void createProgram(int shaderType, int programDefs, int numPointLights, int numSpotLights, int numDirLights, int numShadows2D, int numShadowsCube, int numBlendMapColors);
        virtual void deleteProgram();
        
    };
    
}

#endif /* ProgramRender_h */
