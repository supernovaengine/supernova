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

#define S_PROPERTY_NUMSHADOWS2D 8

#define S_PROPERTY_AMBIENTLIGHT 9

#define S_PROPERTY_NUMPOINTLIGHT 10
#define S_PROPERTY_POINTLIGHT_POS 11
#define S_PROPERTY_POINTLIGHT_POWER 12
#define S_PROPERTY_POINTLIGHT_COLOR 13
#define S_PROPERTY_POINTLIGHT_SHADOWIDX 14

#define S_PROPERTY_NUMSPOTLIGHT 15
#define S_PROPERTY_SPOTLIGHT_POS 16
#define S_PROPERTY_SPOTLIGHT_POWER 17
#define S_PROPERTY_SPOTLIGHT_COLOR 18
#define S_PROPERTY_SPOTLIGHT_TARGET 19
#define S_PROPERTY_SPOTLIGHT_CUTOFF 20
#define S_PROPERTY_SPOTLIGHT_OUTERCUTOFF 21
#define S_PROPERTY_SPOTLIGHT_SHADOWIDX 22

#define S_PROPERTY_NUMDIRLIGHT 23
#define S_PROPERTY_DIRLIGHT_DIR 24
#define S_PROPERTY_DIRLIGHT_POWER 25
#define S_PROPERTY_DIRLIGHT_COLOR 26
#define S_PROPERTY_DIRLIGHT_SHADOWIDX 27

#define S_PROPERTY_FOG_MODE 28
#define S_PROPERTY_FOG_COLOR 29
#define S_PROPERTY_FOG_VISIBILITY 30
#define S_PROPERTY_FOG_DENSITY 31
#define S_PROPERTY_FOG_START 32
#define S_PROPERTY_FOG_END 33

#define S_PROPERTY_SHADOWLIGHT_POS 34
#define S_PROPERTY_SHADOWCAMERA_NEARFAR 35

#define S_PROPERTY_ISPOINTSHADOW 36

#define S_PROPERTY_SHADOWBIAS2D 37
#define S_PROPERTY_SHADOWBIASCUBE 38
#define S_PROPERTY_SHADOWCAMERA_NEARFAR2D 39
#define S_PROPERTY_SHADOWCAMERA_NEARFARCUBE 40

#define S_PROPERTY_NUMCASCADES2D 41

#define S_PROPERTY_BONESMATRIX 42

#define S_PROPERTY_MORPHWEIGHTS 43

#define S_PROPERTY_TERRAINNODEPOS 44
#define S_PROPERTY_TERRAINGLOBALOFFSET 45
#define S_PROPERTY_TERRAINNODESIZE 46

#define S_TEXTURESAMPLER_DIFFUSE 1
#define S_TEXTURESAMPLER_SHADOWMAP2D 2
#define S_TEXTURESAMPLER_SHADOWMAPCUBE 3
#define S_TEXTURESAMPLER_HEIGHTDATA 4

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

        int maxLights;
        int maxShadows2D;
        int maxShadowsCube;


        std::string regexReplace(std::string_view haystack, const std::regex& rx, std::function<std::string(const std::cmatch&)> f);
        std::string replaceAll(std::string source, const std::string from, const std::string to);
        std::string unrollLoops(std::string source);
        
        ProgramRender();
        
    public:
        
        virtual ~ProgramRender();
        
        static std::shared_ptr<ProgramRender> sharedInstance(std::string id);
        static void deleteUnused();
        
        bool isLoaded();

        int getMaxLights();
        int getMaxShadows2D();
        int getMaxShadowsCube();

        virtual void createProgram(int shaderType, int programDefs, int numLights, int numShadows2D, int numShadowsCube);
        virtual void deleteProgram();
        
    };
    
}

#endif /* ProgramRender_h */
