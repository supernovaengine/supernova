#ifndef gles2mesh_h
#define gles2mesh_h

#include "GLES2Header.h"
#include "GLES2Light.h"
#include "GLES2Fog.h"
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

namespace Supernova {

    class GLES2Mesh : public MeshRender {

    private:

        GLES2Light light;
        GLES2Fog fog;
        
        std::shared_ptr<ProgramRender> gProgram;
        
        GLint aPositionHandle;
        GLint aTextureCoordinates;
        GLint aNormal;
        
        GLuint uTextureUnitLocation;
        GLuint uColor;
        GLuint u_mvpMatrix;
        GLuint u_mMatrix;
        GLuint u_nMatrix;
        GLuint uEyePos;

        GLuint useTexture;

        GLuint u_textureRect;
        
        GLuint u_fogMode;
        GLuint u_fogColor;
        GLuint u_fogDensity;
        GLuint u_fogVisibility;
        GLuint u_fogStart;
        GLuint u_fogEnd;

        GLuint vertexBuffer;
        GLuint normalBuffer;
        GLuint uvBuffer;
        
        GLuint vertexBufferSize;
        GLuint normalBufferSize;
        GLuint uvBufferSize;
        
        GLenum usageBuffer;
        
        void useVerticesBuffer();
        void useTexcoordsBuffer();
        void useNormalsBuffer();

    public:
        GLES2Mesh();
        virtual ~GLES2Mesh();
        
        void updateVertices();
        void updateTexcoords();
        void updateNormals();
        void updateIndices();

        bool load();
        bool draw();
        void destroy();
    };
    
}

#endif
