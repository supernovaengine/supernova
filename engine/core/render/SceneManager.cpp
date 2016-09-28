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
    scene->setLights(lights);
}

void SceneManager::setAmbientLight(Vector3 ambientLight){
    scene->setAmbientLight(ambientLight);
}

bool SceneManager::load() {
    instanciateRender();
    return scene->load();
}

bool SceneManager::draw() {
    return scene->draw();
}

bool SceneManager::screenSize(int width, int height){
    instanciateRender();
    return scene->screenSize(width, height);
}
