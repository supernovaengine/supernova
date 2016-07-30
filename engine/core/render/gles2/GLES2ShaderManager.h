#ifndef gles2shadermanager_h
#define gles2shadermanager_h

#include "GLES2Header.h"
#include <vector>

class GLES2ShaderManager {
private:

    typedef struct {
        const GLuint value;
        const char* key;
        int reference;
    } ProgramStore;

    static std::vector<ProgramStore> programs;

    static const char* getVertexShader(const char* name);
    static const char* getFragmentShader(const char* name);

    static GLuint loadShader(GLenum shaderType, const char* pSource);
    static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);

public:
    GLES2ShaderManager();
    virtual ~GLES2ShaderManager();

    static GLuint useProgram(const char* programName);
    static void detatchProgram(const char* programName);

    static void clear();

};

#endif /* gles2_shadermanager_hpp */
