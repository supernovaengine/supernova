#include "MeshManager.h"

#include "Supernova.h"
#include "gles2/GLES2Mesh.h"
#include <stddef.h>


MeshManager::MeshManager() {
    mesh = NULL;
    instanciateRender();
}

MeshManager::~MeshManager() {
    delete mesh;
}

void MeshManager::instanciateRender(){
    if (mesh == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            mesh = new GLES2Mesh();
        }
    }
}

bool MeshManager::load(SceneRender* sceneRender, std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes) {
    instanciateRender();
    return mesh->load(sceneRender, vertices, normals, texcoords, submeshes);
}

bool MeshManager::draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode) {
    return mesh->draw(modelMatrix, normalMatrix, modelViewProjectionMatrix, cameraPosition, mode);
}

void MeshManager::destroy(){
    if (mesh != NULL)
        mesh->destroy();
}
