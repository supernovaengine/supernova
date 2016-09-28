
#ifndef GLES2Program_h
#define GLES2Program_h

#include "GLES2Header.h"
#include "render/ShaderRender.h"
#include <string>


class GLES2Program : public ShaderRender{

private:
    
    GLuint program;
    
    const char* getVertexShader(const char* name);
    const char* getFragmentShader(const char* name);
    
    GLuint loadShader(GLenum shaderType, const char* pSource);
    
    
public:

    void createShader(std::string shaderName);
    void deleteShader();
    
    GLuint getShader();
};

#endif /* GLES2Program_h */
