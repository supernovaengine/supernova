#include "MeshManager.h"

#include "Supernova.h"
#include "render/gles2/GLES2Mesh.h"
#include <stddef.h>


MeshManager::MeshManager() {
    render = NULL;
    instanciateRender();
}

MeshManager::~MeshManager() {
    delete render;
}

void MeshManager::instanciateRender(){
    if (render == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            render = new GLES2Mesh();
        }
    }
}

MeshRender* MeshManager::getRender(){
    instanciateRender();
    return render;
}

bool MeshManager::load() {
    instanciateRender();
    return render->load();
}

bool MeshManager::draw() {
    return render->draw();
}

void MeshManager::destroy(){
    if (render != NULL)
        render->destroy();
}
