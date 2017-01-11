
#ifndef GLES2Program_h
#define GLES2Program_h

#include "GLES2Header.h"
#include "render/ProgramRender.h"
#include <string>


class GLES2Program : public ProgramRender{

private:
    
    GLuint program;
    
    const char* getVertexShader(const char* name);
    const char* getFragmentShader(const char* name);
    
    GLuint loadShader(GLenum shaderType, const char* pSource);
    
    
public:

    void createProgram(std::string shaderName, std::string definitions);
    void deleteProgram();
    
    GLuint getProgram();
};

#endif /* GLES2Program_h */
