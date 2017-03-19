

#ifndef GLES2Point_h
#define GLES2Point_h

#include "GLES2Header.h"
#include <vector>
#include <unordered_map>
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "render/TextureManager.h"
#include "render/ProgramManager.h"
#include "render/PointRender.h"
#include "render/SceneRender.h"
#include "GLES2Light.h"
#include "GLES2Fog.h"


class GLES2Point : public PointRender {
    
private:

    GLES2Light light;
    GLES2Fog fog;
    
    std::shared_ptr<ProgramRender> gProgram;

    GLint aPositionHandle;
    GLint aNormal;
    
    GLint a_PointSize;
    GLint a_slicePos;
    GLint a_pointColor;
    
    GLuint uTextureUnitLocation;
    GLuint u_mvpMatrix;
    GLuint u_mMatrix;
    GLuint u_nMatrix;
    GLuint uEyePos;
    
    GLuint useTexture;
    
    GLuint u_sliceSize;
    GLuint u_textureSize;
    
    GLuint vertexBuffer;
    GLuint normalBuffer;
    GLuint pointSizeBuffer;
    GLuint slicePosBuffer;
    GLuint pointColorBuffer;

    
public:
    GLES2Point();
    virtual ~GLES2Point();
    
    void updatePositions();
    void updateNormals();
    void updatePointSizes();
    void updateSlicesPos();
    void updatePointColors();
    
    bool load();
    bool draw();
    void destroy();
};

#endif /* GLES2Point_h */
