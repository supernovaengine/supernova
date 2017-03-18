#ifndef GLES2Fog_h
#define GLES2Fog_h

#include "GLES2Header.h"
#include "GLES2Program.h"
#include "render/SceneRender.h"

class GLES2Fog {
    
private:
    
    GLES2Program* program;
    
public:
    
    GLES2Fog();
    virtual ~GLES2Fog();
    
    GLuint u_fogMode;
    GLuint u_fogColor;
    GLuint u_fogDensity;
    GLuint u_fogVisibility;
    GLuint u_fogStart;
    GLuint u_fogEnd;
    
    void setProgram(GLES2Program* program);
    void getUniformLocations();
    void setUniformValues(SceneRender* sceneRender);
    
};

#endif /* GLES2Fog_h */
