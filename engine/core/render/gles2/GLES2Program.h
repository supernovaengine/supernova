
#ifndef GLES2Program_h
#define GLES2Program_h

#define MAXSHADOWS_GLES2 12
#define MAXCASCADES_GLES2 3
#define MAXLIGHTS_GLES2 4

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

        virtual void createProgram(int shaderType, int numLights, int numShadows2D, int numShadowsCube, bool hasFog, bool hasTextureCoords, bool hasTextureRect, bool hasTextureCube, bool isSky, bool isText);
        virtual void deleteProgram();
        
        GLuint getProgram();
    };
    
}

#endif /* GLES2Program_h */
