
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

        bool dynamic;

        int primitiveType;

        bool resizeSubmeshes(unsigned int count, Material* material = NULL);

        virtual bool textureLoad();
        //void sortTransparentSubmeshes();
        
    public:
        Mesh();
        virtual ~Mesh();

        int getPrimitiveType();
        void setPrimitiveType(int primitiveType);

        bool isDynamic();

        void addSubmesh(Submesh* submesh);
        void removeSubmesh(Submesh* submesh);

        std::vector<Submesh*> getSubmeshes();

        void updateBuffers();

        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateModelMatrix();

        virtual bool renderLoad(bool shadow);
        virtual bool renderDraw(bool shadow);
        
        virtual bool load();
        virtual void destroy();

    };
    
}

#endif /* mesh_h */
