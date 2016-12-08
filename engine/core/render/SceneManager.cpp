#include "SceneManager.h"

#include "Supernova.h"
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

void SceneManager::setLights(std::vector<Light*> lights){
    instanciateRender();
    this->lights = lights;
    updateLights();
}

void SceneManager::setAmbientLight(Vector3 ambientLight){
    instanciateRender();
    this->ambientLight = ambientLight;
    if (scene)
        scene->setAmbientLight(this->ambientLight);
}

void SceneManager::updateLights(){
    bool needUpdate = false;
    
    for ( int i = 0; i < (int)lights.size(); i++)
        if (!lights[i]->isUpdated()){
            needUpdate = true;
            lights[i]->setUpdated(true);
        }
    
    if (needUpdate)
        if (scene)
            scene->updateLights(this->lights);
}

bool SceneManager::load() {
    instanciateRender();
    return scene->load();
}

bool SceneManager::draw() {
    updateLights();
    return scene->draw();
}

bool SceneManager::viewSize(int x, int y, int width, int height){
    instanciateRender();
    return scene->viewSize(x, y, width, height);
}
