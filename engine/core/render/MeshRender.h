#ifndef MeshRender_h
#define MeshRender_h

#include "math/Vector2.h"
#include "render/SceneRender.h"
#include "render/TextureRender.h"
#include "Submesh.h"
#include <vector>
#include <unordered_map>
#include "math/Rect.h"

namespace Supernova {

    class Mesh;

    class MeshRender {

    private:
        
        Mesh* mesh;

        void checkLighting();
        void checkFog();
        void checkSubmeshProperties();
        void fillMeshProperties();
        
    protected:
        
        bool lighting;
        bool hasfog;
        bool hasTextureRect;
        bool hasTexture;
        bool hasTextureCube;
        
        //-------begin mesh properties-------
        SceneRender* sceneRender;
        std::vector<Vector3>* vertices;
        std::vector<Vector2>* texcoords;
        std::vector<Vector3>* normals;
        std::vector<Submesh*>* submeshes;
        
        Matrix4 modelViewProjectMatrix;
        Matrix4 modelMatrix;
        Matrix4 normalMatrix;
        Vector3 cameraPosition;

        bool isSky;
        bool isDynamic;
        
        int primitiveMode;
        //------------end------------
        
    public:
        
        MeshRender();
        virtual ~MeshRender();

        static void newInstance(MeshRender** render);
        
        void setMesh(Mesh* mesh);
        
        virtual void updateVertices();
        virtual void updateTexcoords();
        virtual void updateNormals();
        virtual void updateIndices();
        
        virtual bool load();
        virtual bool draw();
        virtual void destroy();
        
    };
    
}

#endif /* MeshRender_h */
