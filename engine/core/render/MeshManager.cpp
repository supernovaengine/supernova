#include "MeshManager.h"

#include "Engine.h"
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
        if (Engine::getRenderAPI() == S_GLES2){
            render = new GLES2Mesh();
        }
    }
}

MeshRender* MeshManager::getRender(){
    instanciateRender();
    return render;
}
