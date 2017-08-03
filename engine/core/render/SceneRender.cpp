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
    this->lighting = scene->isUseLight();
    this->childScene = scene->isChildScene();
    this->useDepth = scene->isUseDepth();
    this->useTransparency = scene->isUseTransparency();
}

bool SceneRender::load(){
    fillSceneProperties();
    
    return true;
}
bool SceneRender::draw(){
    fillSceneProperties();

    return true;
}
