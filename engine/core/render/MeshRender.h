#ifndef meshrender_h
#define meshrender_h

#include <vector>
#include "math/Vector3.h"
#include "gles2/GLES2Mesh.h"
#include "Submesh.h"
#include "math/Matrix4.h"
#include "math/Vector4.h"

class MeshRender {
private:
    GLES2Mesh* mesh;

    void instanciateRender();
public:
	MeshRender();
	virtual ~MeshRender();

    bool load(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes);
	bool draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode);
    void destroy();
};

#endif /* meshrender_h */
