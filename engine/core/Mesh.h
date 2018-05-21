
#ifndef mesh_h
#define mesh_h

//
// (c) 2018 Eduardo Doria.
//

#include "ConcreteObject.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "render/ObjectRender.h"
#include "Submesh.h"

namespace Supernova {

    class Mesh: public ConcreteObject {
        
    private:
        void removeAllSubmeshes();
        
    protected:
        ObjectRender* render;
        ObjectRender* shadowRender;

        std::vector<Vector3> vertices;
        std::vector<Vector3> normals;
        std::vector<Vector2> texcoords;
        std::vector<Submesh*> submeshes;
        
        bool skymesh;
        bool textmesh;
        bool dynamic;

        int primitiveType;
        
        void addSubmesh(Submesh* submesh);
        
        void sortTransparentSubmeshes();
        
        void updateVertices();
        void updateNormals();
        void updateTexcoords();
        void updateIndices();
        
    public:
        Mesh();
        virtual ~Mesh();

        int getPrimitiveType();
        std::vector<Vector3> getVertices();
        std::vector<Vector3> getNormals();
        std::vector<Vector2> getTexcoords();
        std::vector<Submesh*> getSubmeshes();
        bool isSky();
        bool isText();
        bool isDynamic();
        
        void setTexcoords(std::vector<Vector2> texcoords);

        void setPrimitiveType(int primitiveType);
        void addVertex(Vector3 vertex);
        void addNormal(Vector3 normal);
        void addTexcoord(Vector2 texcoord);

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
