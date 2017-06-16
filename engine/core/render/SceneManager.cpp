#include "SceneManager.h"

#include "Engine.h"
#include "gles2/GLES2Scene.h"
#include <stddef.h>

using namespace Supernova;

SceneManager::SceneManager() {
    scene = NULL;
    instanciateRender();
}

SceneManager::~SceneManager() {
    delete scene;
}

void SceneManager::instanciateRender(){
    if (scene == NULL){
        if (Engine::getRenderAPI() == S_GLES2){
            scene = new GLES2Scene();
        }
    }
}

SceneRender* SceneManager::getRender(){
    instanciateRender();
    return scene;
}
