
#ifndef GLES2Program_h
#define GLES2Program_h

#define MAXSHADOWS_GLES2 12
#define MAXLIGHTS_GLES2 16

#include "GLES2Header.h"
#include "render/ProgramRender.h"
#include <string>

namespace Supernova {

    class GLES2Program : public ProgramRender{

    private:
        
        GLuint program;

        std::string getVertexShader(int shaderType);
        std::string getFragmentShader(int shaderType);
        
        GLuint loadShader(GLenum shaderType, const char* pSource);
        
        
    public:

        virtual void createProgram(int shaderType, int programDefs, int numPointLights, int numSpotLights, int numDirLights, int numShadows2D, int numShadowsCube, int numBlendMapColors);
        virtual void deleteProgram();
        
        GLuint getProgram();
    };
    
}

#endif /* GLES2Program_h */
