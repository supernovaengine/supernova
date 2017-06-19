#include "SceneRender.h"
#include "math/Angle.h"
#include "Scene.h"
#include "Engine.h"
#include "gles2/GLES2Scene.h"

using namespace Supernova;

SceneRender::SceneRender(){
    lighting = false;
    childScene = false;
    useDepth = false;
    useTransparency = false;

    fog = NULL;
}

SceneRender::~SceneRender(){

}

void SceneRender::newInstance(SceneRender** render){
    if (*render == NULL){
        if (Engine::getRenderAPI() == S_GLES2){
            *render = new GLES2Scene();
        }
    }
}

void SceneRender::setScene(Scene* scene){
    this->scene = scene;
}

void SceneRender::fillSceneProperties(){
    this->lighting = this->lightData.updateLights(scene->getLights(), scene->getAmbientLight());
    this->childScene = scene->isChildScene();
    this->useDepth = scene->isUseDepth();
    this->useTransparency = scene->isUseTransparency();
}

LightData* SceneRender::getLightData(){
    return &lightData;
}

Fog* SceneRender::getFog(){
    return fog;
}

bool SceneRender::load(){
    fillSceneProperties();

    return true;
}
bool SceneRender::draw(){
    fillSceneProperties();

    return true;
}
