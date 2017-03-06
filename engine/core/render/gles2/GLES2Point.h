

#ifndef GLES2Point_h
#define GLES2Point_h

#include "GLES2Header.h"
#include "../../Submesh.h"
#include <vector>
#include <unordered_map>
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "render/TextureManager.h"
#include "render/ProgramManager.h"
#include "render/PointRender.h"
#include "render/SceneRender.h"


class GLES2Point : public PointRender {
    
private:
    
    std::shared_ptr<ProgramRender> gProgram;
    
    
    GLint aPositionHandle;
    GLint aTextureCoordinatesLocation;
    GLint aNormal;
    
    GLint a_PointSize;
    GLint a_spritePos;
    GLint a_pointColor;
    
    GLuint uTextureUnitLocation;
    GLuint uColor;
    GLuint u_mvpMatrix;
    GLuint u_mMatrix;
    GLuint u_nMatrix;
    GLuint uEyePos;
    
    GLuint useTexture;
    
    GLuint u_NumPointLight;
    GLuint u_PointLightPos;
    GLuint u_PointLightPower;
    GLuint u_PointLightColor;
    
    GLuint u_NumSpotLight;
    GLuint u_SpotLightPos;
    GLuint u_SpotLightPower;
    GLuint u_SpotLightColor;
    GLuint u_SpotLightCutOff;
    GLuint u_SpotLightTarget;
    
    GLuint u_NumDirectionalLight;
    GLuint u_DirectionalLightDir;
    GLuint u_DirectionalLightPower;
    GLuint u_DirectionalLightColor;
    
    GLuint u_AmbientLight;
    
    GLuint u_spriteSize;
    GLuint u_textureSize;
    
    GLuint vertexBuffer;
    GLuint normalBuffer;
    GLuint uvBuffer;
    GLuint pointSizeBuffer;
    GLuint spritePosBuffer;
    GLuint pointColorBuffer;

    
public:
    GLES2Point();
    virtual ~GLES2Point();
    
    void updatePositions();
    void updateNormals();
    void updatePointSizes();
    void updateSpritePos();
    void updatePointColors();
    
    bool load();
    bool draw();
    void destroy();
};

#endif /* GLES2Point_h */
