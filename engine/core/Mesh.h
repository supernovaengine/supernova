
#ifndef mesh_h
#define mesh_h

//
// (c) 2018 Eduardo Doria.
//

#include "GraphicObject.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "Submesh.h"
#include <array>

namespace Supernova {

    class Mesh: public GraphicObject {
        
    private:
        void removeAllSubmeshes();
        
    protected:

        std::vector<Submesh*> submeshes;
        
        bool skymesh;
        bool textmesh;
        bool dynamic;

        int primitiveType;

        bool resizeSubmeshes(unsigned int count, Material* material = NULL);
        
        //void sortTransparentSubmeshes();
        
    public:
        Mesh();
        virtual ~Mesh();

        int getPrimitiveType();
        std::vector<Submesh*> getSubmeshes();
        bool isSky();
        bool isText();
        bool isDynamic();

        void setPrimitiveType(int primitiveType);

        void updateBuffers();

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateModelMatrix();
        
        virtual bool textureLoad();
        virtual bool shadowLoad();
        virtual bool shadowDraw();
        
        virtual bool renderDraw();
        
        virtual bool load();
        virtual void destroy();

    };
    
}

#endif /* mesh_h */
