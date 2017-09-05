#ifndef PROGRAM_H
#define PROGRAM_H

#define S_SHADER_POINTS 1
#define S_SHADER_MESH 2
#define S_SHADER_DEPTH_RTT 3

#define S_VERTEXATTRIBUTE_VERTICES 1
#define S_VERTEXATTRIBUTE_TEXTURECOORDS 2
#define S_VERTEXATTRIBUTE_NORMALS 3
#define S_VERTEXATTRIBUTE_POINTSIZES 4
#define S_VERTEXATTRIBUTE_POINTCOLORS 5
#define S_VERTEXATTRIBUTE_TEXTURERECTS 6

#define S_PROPERTY_MVPMATRIX 1
#define S_PROPERTY_MODELMATRIX 2
#define S_PROPERTY_NORMALMATRIX 3
#define S_PROPERTY_DEPTHMVPMATRIX 4
#define S_PROPERTY_CAMERAPOS 5
#define S_PROPERTY_TEXTURERECT 6
#define S_PROPERTY_COLOR 7

#define S_PROPERTY_AMBIENTLIGHT 8

#define S_PROPERTY_NUMPOINTLIGHT 9
#define S_PROPERTY_POINTLIGHT_POS 10
#define S_PROPERTY_POINTLIGHT_POWER 11
#define S_PROPERTY_POINTLIGHT_COLOR 12

#define S_PROPERTY_NUMSPOTLIGHT 13
#define S_PROPERTY_SPOTLIGHT_POS 14
#define S_PROPERTY_SPOTLIGHT_POWER 15
#define S_PROPERTY_SPOTLIGHT_COLOR 16
#define S_PROPERTY_SPOTLIGHT_TARGET 17
#define S_PROPERTY_SPOTLIGHT_CUTOFF 18

#define S_PROPERTY_NUMDIRLIGHT 19
#define S_PROPERTY_DIRLIGHT_DIR 20
#define S_PROPERTY_DIRLIGHT_POWER 21
#define S_PROPERTY_DIRLIGHT_COLOR 22

#define S_PROPERTY_FOG_MODE 23
#define S_PROPERTY_FOG_COLOR 24
#define S_PROPERTY_FOG_VISIBILITY 25
#define S_PROPERTY_FOG_DENSITY 26
#define S_PROPERTY_FOG_START 27
#define S_PROPERTY_FOG_END 28

#include "render/ProgramRender.h"
#include <string>
#include <vector>

namespace Supernova{

    class Program{
    private:
        std::shared_ptr<ProgramRender> programRender;
        
        int shaderType;
        bool hasLight;
        bool hasFog;
        bool hasTextureCoords;
        bool hasTextureRect;
        bool hasTextureCube;
        bool isSky;
        bool isText;
        
        std::vector<int> shaderVertexAttributes;
        std::vector<int> shaderProperties;

    public:
        Program();

        void setShader(int shaderType);
        void setDefinitions(bool hasLight = false, bool hasFog = false, bool hasTexture = false, bool hasTextureRect = false, bool hasTextureCube = false, bool isSky = false, bool isText = false);
        
        bool existVertexAttribute(int vertexAttribute);
        bool existProperty(int property);
        
        bool load();
        void destroy();
        
        Program(const Program& p);
        virtual ~Program();

        Program& operator = (const Program& p);

        std::shared_ptr<ProgramRender> getProgramRender();

    };

}


#endif //PROGRAM_H
