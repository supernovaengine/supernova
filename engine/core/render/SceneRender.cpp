#include "SceneRender.h"

#include "Supernova.h"
#include <stddef.h>


SceneRender::SceneRender() {
    scene = NULL;
    instanciateRender();
}

SceneRender::~SceneRender() {
    delete scene;
}

void SceneRender::instanciateRender(){
    if (scene == NULL){
        if (Supernova::getRenderAPI() == S_GLES2){
            scene = new GLES2Scene();
        }
    }
}

void SceneRender::setLights(std::vector<Light*> lights){
    scene->setLights(lights);
}

void SceneRender::setAmbientLight(Vector3 ambientLight){
    scene->setAmbientLight(ambientLight);
}

bool SceneRender::load() {
    instanciateRender();
    return scene->load();
}

bool SceneRender::draw() {
    return scene->draw();
}

bool SceneRender::screenSize(int width, int height){
    instanciateRender();
    return scene->screenSize(width, height);
}
