#ifndef gles2draw_h
#define gles2draw_h

#include "GLES2Header.h"
#include "../../Submesh.h"
#include <vector>
#include <unordered_map>
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "render/TextureManager.h"
#include "render/ProgramManager.h"
#include "render/DrawRender.h"
#include "render/SceneRender.h"


class GLES2Draw : public DrawRender {
    
    typedef struct {
        GLuint indiceBuffer;
        std::shared_ptr<TextureRender> texture;
        unsigned int indicesSizes;
        bool textured;
    } SubmeshStruct;
    
private:
    std::shared_ptr<ProgramRender> gProgram;

    
    GLint aPositionHandle;
    GLint aTextureCoordinatesLocation;
    GLint aNormal;
    
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

    GLuint vertexBuffer;
    GLuint normalBuffer;
    GLuint uvBuffer;

    static GLuint emptyTexture; //For web bug only
    static bool emptyTextureLoaded;
    
    bool lighting;

    int primitiveSize;
    
    //--Use for mesh draw
    std::unordered_map<Submesh*, SubmeshStruct> submeshesGles;
    //--Use for points draw
    std::shared_ptr<TextureRender> texture;
    bool textured;
    //--end

    bool loaded;
    
    void checkLighting();

public:
	GLES2Draw();
	virtual ~GLES2Draw();

    bool load();
	bool draw();
    void destroy();
};

#endif
