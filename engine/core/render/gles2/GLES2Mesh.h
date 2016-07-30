#ifndef gles2primitive_h
#define gles2primitive_h

#include "GLES2Header.h"
#include "../../Submesh.h"
#include <vector>
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "math/Vector3.h"


class GLES2Mesh {
private:
    GLuint gProgram;

    GLuint aPositionHandle;
    GLuint aTextureCoordinatesLocation;
    GLuint uTextureUnitLocation;
    GLuint uColor;
    GLuint u_mvpMatrix;
    GLuint u_mMatrix;
    GLuint u_nMatrix;
    GLuint aNormal;
    GLuint uEyePos;

    GLuint useLighting;
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


    int primitiveSize;

    std::vector<Submesh> submeshes;
    //TODO: struct submesh
    std::vector<GLuint> indiceBuffer;
    std::vector<GLuint> gTexture;
    std::vector<unsigned int> indicesSizes;
    std::vector<bool> textured;


    std::vector<GLfloat> gPrimitiveVertices;
    std::vector<GLfloat> gNormals;
    std::vector<GLfloat> guvMapping;

    const char* programName;


    bool lighting;
    bool loaded;
public:
	GLES2Mesh();
	virtual ~GLES2Mesh();

    bool load(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes);
	bool draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode);
    void destroy();
};

#endif
