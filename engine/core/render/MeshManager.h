#ifndef meshmanager_h
#define meshmanager_h

#include <vector>
#include "math/Vector3.h"
#include "Submesh.h"
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "render/MeshRender.h"
#include "render/SceneRender.h"


class MeshManager {
private:
    MeshRender* mesh;

    void instanciateRender();
public:
	MeshManager();
	virtual ~MeshManager();

    bool load(SceneRender* sceneRender, std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh*>* submeshes);
	bool draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode);
    void destroy();
};

#endif /* meshmanager_h */
