#ifndef gles2primitive_h
#define gles2primitive_h

#include "GLES2Header.h"
#include "../../Submesh.h"
#include <vector>
#include <unordered_map>
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "render/TextureManager.h"
#include "render/ProgramManager.h"
#include "render/MeshRender.h"
#include "render/SceneRender.h"


class GLES2Mesh : public MeshRender {
    
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
    bool isSky;

    int primitiveSize;

    std::vector<Submesh*>* submeshes;
    
    std::unordered_map<Submesh*, SubmeshStruct> submeshesGles;

    std::vector<GLfloat> gPrimitiveVertices;
    std::vector<GLfloat> gNormals;
    std::vector<GLfloat> guvMapping;
    
    SceneRender* sceneRender;

    bool loaded;
    
    void checkLighting();

public:
	GLES2Mesh();
	virtual ~GLES2Mesh();

    bool load(SceneRender* sceneRender, std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh*>* submeshes, bool isSky);
	bool draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode);
    void destroy();
};

#endif
