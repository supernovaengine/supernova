#include "SceneManager.h"

#include "Supernova.h"
#include <stddef.h>


SceneManager::SceneManager() {
    scene = NULL;
    childScene = false;
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
    updateAmbientLight();
}

void SceneManager::updateLights(){
    if (scene)
        scene->setLights(this->lights);
}

void SceneManager::updateAmbientLight(){
    if (scene)
        scene->setAmbientLight(this->ambientLight);
}

bool SceneManager::isChildScene(){
    return childScene;
}

void SceneManager::setChildScene(bool childScene){
    this->childScene = childScene;
}

bool SceneManager::load() {
    instanciateRender();
    return scene->load(childScene);
}

bool SceneManager::draw() {
    updateLights();
    return scene->draw(childScene);
}

bool SceneManager::viewSize(int x, int y, int width, int height){
    instanciateRender();
    return scene->viewSize(x, y, width, height);
}
