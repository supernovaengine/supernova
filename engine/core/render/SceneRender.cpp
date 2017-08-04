#include "SceneRender.h"
#include "math/Angle.h"
#include "Engine.h"
#include "gles2/GLES2Scene.h"

using namespace Supernova;

SceneRender::SceneRender(){
    useLight = false;
    childScene = false;
    useDepth = false;
    useTransparency = false;
}

SceneRender::~SceneRender(){

}

SceneRender* SceneRender::newInstance(){
    if (Engine::getRenderAPI() == S_GLES2){
        return new GLES2Scene();
    }

    return NULL;
}

void SceneRender::setUseLight(bool useLight){
    this->useLight = useLight;
}

void SceneRender::setChildScene(bool childScene){
    this->childScene = childScene;
}

void SceneRender::setUseDepth(bool useDepth){
    this->useDepth = useDepth;
}

void SceneRender::setUseTransparency(bool useTransparency){
    this->useTransparency = useTransparency;
}

bool SceneRender::load(){
    return true;
}
bool SceneRender::draw(){
    return true;
}
