
#ifndef GLES2Program_h
#define GLES2Program_h

#include "GLES2Header.h"
#include "render/ProgramRender.h"
#include <string>


class GLES2Program : public ProgramRender{

private:
    
    GLuint program;
    
    std::string getVertexShader(std::string name);
    std::string getFragmentShader(std::string name);
    
    GLuint loadShader(GLenum shaderType, const char* pSource);
    
    
public:

    void createProgram(std::string shaderName, std::string definitions);
    void deleteProgram();
    
    GLuint getProgram();
};

#endif /* GLES2Program_h */
