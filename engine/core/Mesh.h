
#ifndef mesh_h
#define mesh_h

//
// (c) 2018 Eduardo Doria.
//

#include "GraphicObject.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "SubMesh.h"
#include <array>

namespace Supernova {

    class Mesh: public GraphicObject {
        
    private:
        void removeAllSubMeshes();
        
    protected:

        std::vector<SubMesh*> submeshes;

        std::vector<Matrix4> bonesMatrix;
        
        bool skymesh;
        bool textmesh;
        bool skinning;
        bool dynamic;

        int primitiveType;
        
        void addSubMesh(SubMesh* submesh);
        
        //void sortTransparentSubMeshes();
        
    public:
        Mesh();
        virtual ~Mesh();

        int getPrimitiveType();
        std::vector<SubMesh*> getSubMeshes();
        bool isSky();
        bool isText();
        bool isDynamic();

        void setPrimitiveType(int primitiveType);

        void updateBuffers();

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateMatrix();
        
        virtual bool textureLoad();
        virtual bool shadowLoad();
        virtual bool shadowDraw();
        
        virtual bool renderDraw();
        
        virtual bool load();
        virtual void destroy();

    };
    
}

#endif /* mesh_h */
