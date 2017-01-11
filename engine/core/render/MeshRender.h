#ifndef MeshRender_h
#define MeshRender_h

#include "math/Vector2.h"


class MeshRender {
    

public:
    
    inline virtual ~MeshRender(){}
    
    virtual bool load(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes) = 0;
    virtual bool draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode) = 0;
    virtual void destroy() = 0;
    
};

#endif /* MeshRender_h */
