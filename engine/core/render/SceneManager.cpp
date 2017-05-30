#include "SceneManager.h"

#include "Supernova.h"
#include "gles2/GLES2Scene.h"
#include <stddef.h>


SceneManager::SceneManager() {
    scene = NULL;
    instanciateRender();
}

SceneManager::~SceneManager() {
    delete scene;
}

void SceneManager::instanciateRender(){
    if (scene == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            scene = new GLES2Scene();
        }
    }
}

SceneRender* SceneManager::getRender(){
    instanciateRender();
    return scene;
}

bool SceneManager::load() {
    instanciateRender();
    return scene->load();
}

bool SceneManager::draw() {
    return scene->draw();
}

bool SceneManager::viewSize(Rect rect){
    instanciateRender();
    return scene->viewSize(rect);
}
