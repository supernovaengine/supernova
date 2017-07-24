#ifndef PROGRAM_H
#define PROGRAM_H

#define S_SHADER_POINTS 1
#define S_SHADER_MESH 2

#define S_VERTEXATTRIBUTE_VERTICES 1
#define S_VERTEXATTRIBUTE_NORMALS 3
#define S_VERTEXATTRIBUTE_POINTSIZES 10
#define S_VERTEXATTRIBUTE_POINTCOLORS 11
#define S_VERTEXATTRIBUTE_TEXTURERECTS 12

#define S_PROPERTY_MVPMATRIX 1
#define S_PROPERTY_MODELMATRIX 2
#define S_PROPERTY_NORMALMATRIX 3
#define S_PROPERTY_CAMERAPOS 4

#include "render/ProgramRender.h"
#include <string>
#include <vector>

namespace Supernova{

    class Program{
    private:
        std::shared_ptr<ProgramRender> programRender;
        
        std::string shader;
        std::string definitions;
        
        int shaderType;
        bool hasLight;
        bool hasFog;
        bool hasTextureRect;
        
        std::vector<int> shaderVertexAttributes;
        std::vector<int> shaderProperties;

    public:
        Program();
        Program(std::string shader, std::string definitions);
        
        void setShader(std::string shader);
        void setShader(int shaderType);
        void setDefinitions(std::string definitions);
        void setDefinitions(bool hasLight, bool hasFog, bool hasTextureRect);
        
        std::string getShader();
        std::string getDefinitions();
        
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
