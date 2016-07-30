#include "MeshRender.h"

#include "Supernova.h"
#include <stddef.h>


MeshRender::MeshRender() {
    mesh = NULL;
    instanciateRender();
}

MeshRender::~MeshRender() {
    delete mesh;
}

void MeshRender::instanciateRender(){
    if (mesh == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            mesh = new GLES2Mesh();
        }
    }
}

bool MeshRender::load(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Vector2> texcoords, std::vector<Submesh> submeshes) {
    instanciateRender();
    return mesh->load(vertices, normals, texcoords, submeshes);
}

bool MeshRender::draw(Matrix4* modelMatrix, Matrix4* normalMatrix, Matrix4* modelViewProjectionMatrix, Vector3* cameraPosition, int mode) {
    return mesh->draw(modelMatrix, normalMatrix, modelViewProjectionMatrix, cameraPosition, mode);
}

void MeshRender::destroy(){
    if (mesh != NULL)
        mesh->destroy();
}
